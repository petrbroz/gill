#ifndef GILL_CORE_PRIMITIVE_H_
#define GILL_CORE_PRIMITIVE_H_

#include "transform.h"
#include "bbox.h"
#include "ray.h"
#include "vector.h"
#include "mesh.h"

namespace gill { namespace core {

class Primitive;
typedef std::shared_ptr<Primitive> PrimitiveRef;

/**
 * Geometry, material and transform of a specific scene object.
 */
class Primitive {
protected:
    const MeshRef _mesh;
    /// Transformation from local to world coordinate system
    const Transform *_ltow;
    /// Transformation from world to local coordinate system
    const Transform *_wtol;
public:

    /**
     * Collection of data related to a specific primitive intersection.
     */
    struct Intersection {
        PrimitiveRef primitive;
        Mesh::Intersection mesh_isec;
        float u, v;
        Vector dpdu, dpdv;
        Normal dndu, dndv;
    };

    Primitive(const MeshRef mesh, const Transform *ltow, const Transform *wtol);
    BBox local_bounds() const;
    BBox world_bounds() const;
    bool intersect(const Ray &ray, float &t, Intersection *i) const;
};

}}

#endif
