#ifndef GILL_GEOMETRY_MESH_H_
#define GILL_GEOMETRY_MESH_H_

#include <vector>
#include <iostream>

#include "core/bbox.h"
#include "core/geometry.h"
#include "core/kdtree.h"
#include "core/ray.h"
#include "core/vector.h"
#include "core/intersection.h"

using namespace gill::core;

namespace gill { namespace geometry {

/**
 * Geometry represented as a triangular mesh.
 * The geometry always works with local coordinate system. It is the role of gill::core::Primitive
 * to position/orient/scale an instance of the geometry in the scene.
 */
class Mesh : public Geometry {
public:

    /**
     * Standalone representation of a mesh triangle.
     * While it may not be neccessary to use such structure, it will allow for a straightforward
     * integration with accelerators, and for performance improvements through additional caching.
     */
    struct Triangle {
        int i1, i2, i3;

        BBox bounds(Mesh *mesh) const;
        bool intersect(Mesh *mesh, const Ray &ray, float &t, Intersection *isec) const;
        friend std::ostream& operator<<(std::ostream& out, const Mesh& mesh);
    };

    BBox bounds() const override;
    bool intersect(const Ray &ray, float &t, Intersection *i) const override;
    int num_faces() const { return _triangles.size(); }
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
    std::unique_ptr<KdTree> _accelerator;
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
