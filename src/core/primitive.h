#ifndef GILL_CORE_PRIMITIVE_H_
#define GILL_CORE_PRIMITIVE_H_

#include <iostream>

#include "core/bbox.h"
#include "core/geometry.h"
#include "core/ray.h"
#include "core/transform.h"
#include "core/vector.h"
#include "core/intersection.h"
#include "core/material.h"

namespace gill { namespace core {

/**
 * Scene entity encapsulating a geometry, material, and a local-to-world transformation.
 */
class Primitive {
public:
    Primitive(std::shared_ptr<Geometry> geom, std::shared_ptr<Material> material,
            std::shared_ptr<Transform> ltow, std::shared_ptr<Transform> wtol);
    BBox local_bounds() const;
    BBox bounds() const;
    bool intersect(const Ray &ray, float &t, Intersection *i) const;
    int num_faces() const { return _geom->num_faces(); }
    friend std::ostream& operator<<(std::ostream &out, const Primitive &primitive);

protected:
    std::shared_ptr<Geometry> _geom;
    std::shared_ptr<Material> _material;
    std::shared_ptr<Transform> _ltow; /// Transformation from local to world coordinate system
    std::shared_ptr<Transform> _wtol; /// Transformation from world to local coordinate system
};

inline std::ostream& operator<<(std::ostream &out, const Primitive &primitive) {
    out << "{";
    out << "\"geometry\":" << primitive._geom << ",";
    out << "\"local_to_world\":" << *(primitive._ltow) << ",";
    out << "\"world_to_local\":" << *(primitive._wtol);
    out << "}";
    return out;
}

}}

#endif
