#ifndef GILL_CORE_MESH_H_
#define GILL_CORE_MESH_H_

#include <vector>

#include "vector.h"
#include "ray.h"
#include "bbox.h"

namespace gill { namespace core {

struct Triangle;

/**
 * Geometry represented as a triangular mesh.
 * The geometry always works with local coordinate system. It is the purpose of Primitive
 * to position/orient/scale an instance of the geometry in the scene.
 */
class Mesh {

    /**
     * Collection of data related to a particular mesh intersection.
     */
    struct Intersection {
        const Mesh *mesh;
        Point p;
        Normal n;
        float t;
        float u, v;
        Vector dpdu, dpdv;
        Normal dndu, dndv;
    };

    /**
     * Standalone representation of a mesh triangle.
     * While it may not be neccessary to use such structure, it will allow for a straightforward
     * integration with accelerators, and for performance improvements through additional caching.
     */
    struct Triangle {
        Mesh *mesh;
        int i1, i2, i3;

        BBox bounds() const;
        bool intersect(const Ray &ray, Intersection *i) const;
    };

    std::vector<Triangle> triangles;
    std::vector<Point> vertices;
    std::vector<Normal> normals;
    BBox bbox;

    Mesh() {}
public:
    BBox bounds() const;
    bool intersect(const Ray &ray, Intersection *i) const;

    static Mesh * from_obj_file(const char *filename);
};

}}

#endif
