#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <vector>
#include <ctime>

#include "geometry/mesh.h"

namespace gill { namespace geometry {

const int MeshFileMagicNum = 0xdeadbeef;

using namespace std;

BBox Mesh::Triangle::bounds(Mesh *mesh) const {
    BBox bbox(mesh->_vertices[i1]);
    bbox += mesh->_vertices[i2];
    bbox += mesh->_vertices[i3];
    return bbox;
}

bool Mesh::Triangle::intersect(Mesh *mesh, const Ray &ray, float &t, Intersection *i) const {
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

    float _t = dot(e2, Q) * inv_det;
    if (_t < t) {
        t = _t;
        if (i) {
            i->p = ray(t);
            i->n = normalize(cross(e1, e2));
        }
        return true;
    } else {
        return false;
    }
}

BBox Mesh::bounds() const {
    return _bounds;
}

bool Mesh::intersect(const Ray &ray, float &t, Intersection *isec) const {
#ifdef BRUTE_FORCE
    for (const Triangle &triangle : _triangles) {
        if (triangle.intersect(ray, t, nullptr)) {
            return true;
        }
    }
    return false;
#else
    return _accelerator->intersect(ray, t, isec);
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
        _triangles.insert(_triangles.end(), t, t + tcount);
        delete[] t;
    }

    fclose(f);
}

shared_ptr<Mesh> Mesh::from_obj_file(const char *filename) {
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
            mesh->_triangles.push_back({stoi(match[1]) - 1, stoi(match[2]) - 1, stoi(match[3]) - 1});
        }
    }
    Mesh * mesh_ptr = mesh.get();
    mesh->_accelerator.reset(new KdTree(mesh->_triangles.size(), 80.0, 10.0, 8, 32,
        [mesh_ptr](uint32_t i) {
            const Triangle &tri = mesh_ptr->_triangles[i];
            return tri.bounds(mesh_ptr);
        },
        [mesh_ptr](uint32_t i, const Ray &ray, float &t, Intersection *isec) {
            const Triangle &tri = mesh_ptr->_triangles[i];
            return tri.intersect(mesh_ptr, ray, t, isec);
        }));
    mesh->_bounds = mesh->_accelerator->bounds();

    string mesh_file(filename);
    mesh_file += ".mesh";
    mesh->save(mesh_file.c_str());

    string tree_file(filename);
    tree_file += ".kdtree";
    mesh->_accelerator->save(tree_file.c_str());

    return mesh;
}

shared_ptr<Mesh> Mesh::from_cache_file(const char *filename) {
    auto mesh = make_shared<Mesh>();
    string mesh_file(filename);
    mesh_file += ".mesh";
    mesh->load(mesh_file.c_str());
    string tree_file(filename);
    tree_file += ".kdtree";
    Mesh * mesh_ptr = mesh.get();
    mesh->_accelerator.reset(new KdTree(tree_file.c_str(),
        [mesh_ptr](uint32_t i) {
            const Triangle &tri = mesh_ptr->_triangles[i];
            return tri.bounds(mesh_ptr);
        },
        [mesh_ptr](uint32_t i, const Ray &ray, float &t, Intersection *isec) {
            const Triangle &tri = mesh_ptr->_triangles[i];
            return tri.intersect(mesh_ptr, ray, t, isec);
        }));
    return mesh;
}

int Mesh::num_triangles() const {
    return _triangles.size();
}

}}
