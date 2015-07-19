#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <vector>
#include <ctime>

#include "core/mesh.h"

namespace gill { namespace core {

const int MeshFileMagicNum = 0xdeadbeef;

using namespace std;

BBox Mesh::Triangle::bounds() const {
    BBox bbox(mesh->_vertices[i1]);
    bbox += mesh->_vertices[i2];
    bbox += mesh->_vertices[i3];
    return bbox;
}

bool Mesh::Triangle::intersect(const Ray &ray, float &t, Mesh::Triangle::Intersection *i) const {
    Point p0 = mesh->_vertices[i1], p1 = mesh->_vertices[i2], p2 = mesh->_vertices[i3];
    Vector e1 = p1 - p0, e2 = p2 - p0;
    Vector P = cross(ray.d, e2);
    float det = dot(e1, P);
    if (almost_zero(det)) {
        return false;
    }

    float inv_det = 1.0 / det;
    Vector T = ray.o - p0;
    float u = dot(T, P) * inv_det;
    if (u < 0.0 || u > 1.0) {
        return false;
    }

    Vector Q = cross(T, e1);
    float v = dot(ray.d, Q) * inv_det;
    if (v < 0.0 || u + v > 1.0) {
        return false;
    }

    t = dot(e2, Q) * inv_det;
    if (i) {
        i->p = ray(t);
        i->n = normalize(cross(e1, e2));
    }

    return true;
}

BBox Mesh::bounds() const {
    return _bounds;
}

bool Mesh::intersect(const Ray &ray, float &t, Mesh::Intersection *mi) const {
#ifdef BRUTE_FORCE
    for (const Triangle &triangle : _triangles) {
        if (triangle.intersect(ray, t, nullptr)) {
            return true;
        }
    }
    return false;
#else
    Mesh::Triangle::Intersection *ti = nullptr;
    if (mi) {
        ti = &(mi->ti);
    }
    return _accelerator->intersect(_triangles, ray, t, ti);
#endif
}

void Mesh::save(const char *filename) {
    auto f = fopen(filename, "wb");
    fwrite(&MeshFileMagicNum, sizeof(MeshFileMagicNum), 1, f);
    fwrite(&_bounds, sizeof(_bounds), 1, f);
    size_t vcount = _vertices.size();
    fwrite(&vcount, sizeof(vcount), 1, f);
    size_t ncount = _normals.size();
    fwrite(&ncount, sizeof(ncount), 1, f);
    size_t tcount = _triangles.size();
    fwrite(&tcount, sizeof(tcount), 1, f);

    if (vcount > 0) {
        fwrite(&_vertices[0], sizeof(Point), vcount, f);
    }
    if (ncount > 0) {
        fwrite(&_normals[0], sizeof(Normal), ncount, f);
    }
    if (tcount > 0) {
        fwrite(&_triangles[0], sizeof(Triangle), tcount, f);
    }
    fclose(f);
}

void Mesh::load(const char *filename) {
    auto f = fopen(filename, "rb");
    int magic;
    fread(&magic, sizeof(MeshFileMagicNum), 1, f);
    if (magic != MeshFileMagicNum) {
        std::cerr << "Mesh::load - incorrect magic number" << std::endl;
        exit(1);
    }
    fread(&_bounds, sizeof(BBox), 1, f);

    size_t vcount, ncount, tcount;
    fread(&vcount, sizeof(size_t), 1, f);
    fread(&ncount, sizeof(size_t), 1, f);
    fread(&tcount, sizeof(size_t), 1, f);
    _vertices.clear();
    if (vcount > 0) {
        Point *v = new Point[vcount];
        fread(v, sizeof(Point), vcount, f);
        _vertices.insert(_vertices.end(), v, v + vcount);
        delete[] v;
    }
    _normals.clear();
    if (ncount > 0) {
        Normal *n = new Normal[ncount];
        fread(n, sizeof(Normal), ncount, f);
        _normals.insert(_normals.end(), n, n + ncount);
        delete[] n;
    }
    _triangles.clear();
    if (tcount > 0) {
        Triangle *t = new Triangle[tcount];
        fread(t, sizeof(Triangle), tcount, f);
        for (int i = 0; i < tcount; i++) {
            t[i].mesh = this;
        }
        _triangles.insert(_triangles.end(), t, t + tcount);
        delete[] t;
    }

    fclose(f);
}

shared_ptr<Mesh> Mesh::from_obj_file(const char *filename) {
    auto begin_time = clock();
    ifstream input(filename);
    regex vertex_re("v ([0-9.e-]+) ([0-9.e-]+) ([0-9.e-]+)");
    regex face_re("f ([0-9]*)(?:/[0-9]*)?(?:/[0-9]*)? ([0-9]*)(?:/[0-9]*)?(?:/[0-9]*)? ([0-9]*)(?:/[0-9]*)?(?:/[0-9]*)?");
    string line;
    smatch match;
    auto mesh = make_shared<Mesh>();
    while (getline(input, line)) {
        if (regex_match(line, match, vertex_re)) {
            mesh->_vertices.push_back({stof(match[1]), stof(match[2]), stof(match[3])});
        } else if (regex_match(line, match, face_re)) {
            mesh->_triangles.push_back({mesh.get(), stoi(match[1]) - 1, stoi(match[2]) - 1, stoi(match[3]) - 1});
        }
    }
    mesh->_accelerator.reset(new KdTree<Mesh::Triangle>(mesh->_triangles, 80.0, 10.0, 8, 32));
    mesh->_bounds = mesh->_accelerator->bounds();

    string mesh_file(filename);
    mesh_file += ".mesh";
    mesh->save(mesh_file.c_str());

    string tree_file(filename);
    tree_file += ".kdtree";
    mesh->_accelerator->save(tree_file.c_str());

    auto end_time = clock();
    cerr << "load_time:" << float(end_time - begin_time) / CLOCKS_PER_SEC << "s" << endl;
    cerr << "triangles:" << mesh->_triangles.size() << endl;
    return mesh;
}

shared_ptr<Mesh> Mesh::from_cache_file(const char *filename) {
    auto begin_time = clock();

    auto mesh = make_shared<Mesh>();
    string mesh_file(filename);
    mesh_file += ".mesh";
    mesh->load(mesh_file.c_str());
    string tree_file(filename);
    tree_file += ".kdtree";
    mesh->_accelerator.reset(new KdTree<Mesh::Triangle>(tree_file.c_str()));

    auto end_time = clock();
    cerr << "load_time:" << float(end_time - begin_time) / CLOCKS_PER_SEC << "s" << endl;
    cerr << "triangles:" << mesh->_triangles.size() << endl;
    return mesh;
}

}}
