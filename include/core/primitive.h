#ifndef GILL_CORE_PRIMITIVE_H_
#define GILL_CORE_PRIMITIVE_H_

#include "transform.h"
#include "bbox.h"
#include "ray.h"
#include "vector.h"
#include "mesh.h"

namespace gill { namespace core {

/**
 * Geometry, material and transform of a specific scene object.
 */
class Primitive {
protected:
    const Mesh *mesh;
    /// Transformation from local to world coordinate system
    const Transform *ltow;
    /// Transformation from world to local coordinate system
    const Transform *wtol;
public:

    /**
     * Collection of data related to a specific primitive intersection.
     */
    struct Intersection {
        Primitive *primitive;
        Mesh::Intersection mesh_isec;
        float u, v;
        Vector dpdu, dpdv;
        Normal dndu, dndv;
    };

    Primitive(const Mesh *mesh, const Transform *localToWorld, const Transform *worldToLocal);
    BBox localBounds() const;
    BBox worldBounds() const;
    bool intersect(const Ray &ray, float &t, Intersection *i) const;
};

}}

#endif
