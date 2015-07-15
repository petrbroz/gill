#ifndef GILL_CORE_MESH_H_
#define GILL_CORE_MESH_H_

#include <vector>
#include <iostream>
#include "vector.h"
#include "ray.h"
#include "bbox.h"
#include "kdtree.h"

namespace gill { namespace core {

/**
 * Geometry represented as a triangular mesh.
 * The geometry always works with local coordinate system. It is the purpose of Primitive
 * to position/orient/scale an instance of the geometry in the scene.
 */
class Mesh {
public:

    /**
     * Standalone representation of a mesh triangle.
     * While it may not be neccessary to use such structure, it will allow for a straightforward
     * integration with accelerators, and for performance improvements through additional caching.
     */
    struct Triangle {
        struct Intersection {
            Point p;
            Normal n;
        };

        Mesh *mesh;
        int i1, i2, i3;

        BBox bounds() const;
        bool intersect(const Ray &ray, float &t, Intersection *i) const;
        friend std::ostream& operator<<(std::ostream& out, const Mesh& mesh);
    };

    /**
     * Collection of data related to a particular mesh intersection.
     */
    struct Intersection {
        std::shared_ptr<Mesh> mesh;
        Triangle::Intersection ti;
        float t;
        float u, v;
        Vector dpdu, dpdv;
        Normal dndu, dndv;
    };

    BBox bounds() const;
    bool intersect(const Ray &ray, float &t, Intersection *i) const;
    void save(const char *filename);
    void load(const char *filename);
    static std::shared_ptr<Mesh> from_obj_file(const char *filename);
    static std::shared_ptr<Mesh> from_cache_file(const char *filename);
    friend std::ostream& operator<<(std::ostream& out, const Mesh& mesh);

protected:
    std::vector<Triangle> _triangles;
    std::vector<Point> _vertices;
    std::vector<Normal> _normals;
    BBox _bounds;
    std::unique_ptr<KdTree<Triangle>> _accelerator;
};

inline std::ostream& operator<<(std::ostream& out, const Mesh& mesh) {
    out << "{";
    out << "\"vertices\":[";
    for (auto v = mesh._vertices.begin(); v != mesh._vertices.end(); ++v) {
        if (v != mesh._vertices.begin()) {
            out << ",";
        }
        out << v->x << "," << v->y << "," << v->z;
    }
    out << "],";
    out << "\"triangles\":[";
    for (auto t = mesh._triangles.begin(); t != mesh._triangles.end(); ++t) {
        if (t != mesh._triangles.begin()) {
            out << ",";
        }
        out << t->i1 << "," << t->i2 << "," << t->i3;
    }
    out << "],";
    out << "\"bounds\":" << mesh._bounds;
    out << "}";
    return out;
}

}}

#endif
