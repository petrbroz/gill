#ifndef GILL_CORE_MESH_H_
#define GILL_CORE_MESH_H_

#include <vector>
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

        std::shared_ptr<Mesh> mesh;
        int i1, i2, i3;

        BBox bounds() const;
        bool intersect(const Ray &ray, float &t, Intersection *i) const;
    };

    /**
     * Collection of data related to a particular mesh intersection.
     */
    struct Intersection {
        std::shared_ptr<Mesh> mesh;
        Triangle::Intersection triangle_isec;
        float t;
        float u, v;
        Vector dpdu, dpdv;
        Normal dndu, dndv;
    };

    BBox bounds() const;
    bool intersect(const Ray &ray, float &t, Intersection *i) const;
    static std::shared_ptr<Mesh> from_obj_file(const char *filename);

protected:
    std::vector<Triangle> _triangles;
    std::vector<Point> _vertices;
    std::vector<Normal> _normals;
    BBox _bounds;
    std::unique_ptr<KdTree<Triangle>> _accelerator;
};

}}

#endif
