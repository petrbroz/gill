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
public:

    /**
     * Collection of data related to a specific primitive intersection.
     */
    struct Intersection {
        Primitive *primitive;
        Mesh::Intersection mi;
        float u, v;
        Vector dpdu, dpdv;
        Normal dndu, dndv;
    };

    Primitive(std::shared_ptr<Mesh> mesh, std::shared_ptr<Transform> ltow, std::shared_ptr<Transform> wtol);
    BBox local_bounds() const;
    BBox bounds() const;
    bool intersect(const Ray &ray, float &t, Intersection *i) const;

protected:
    std::shared_ptr<Mesh> _mesh;
    /// Transformation from local to world coordinate system
    std::shared_ptr<Transform> _ltow;
    /// Transformation from world to local coordinate system
    std::shared_ptr<Transform> _wtol;
};

}}

#endif
