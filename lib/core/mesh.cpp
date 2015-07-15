#include "mesh.h"

#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <vector>
#include <ctime>

namespace gill { namespace core {

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
            mesh->_triangles.push_back({mesh, stoi(match[1]) - 1, stoi(match[2]) - 1, stoi(match[3]) - 1});
        }
    }
    mesh->_accelerator.reset(new KdTree<Mesh::Triangle>(mesh->_triangles, 80.0, 10.0, 8, 32));
    mesh->_bounds = mesh->_accelerator->bounds();
    auto end_time = clock();
    cerr << "Mesh::from_obj_file " << float(end_time - begin_time) / CLOCKS_PER_SEC << "s" << endl;
    return mesh;
}

}}
