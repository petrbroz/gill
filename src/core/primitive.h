#ifndef GILL_CORE_PRIMITIVE_H_
#define GILL_CORE_PRIMITIVE_H_

#include <iostream>

#include "core/transform.h"
#include "core/bbox.h"
#include "core/ray.h"
#include "core/vector.h"
#include "geometry/geometry.h"

using namespace gill::geometry;

namespace gill { namespace core {

/**
 * Scene entity encapsulating a geometry, material, and a local-to-world transformation.
 */
class Primitive {
public:

    /**
     * Collection of data related to a specific primitive intersection.
     */
    struct Intersection {
        Primitive *primitive;
        Geometry::Intersection gi;
        float u, v;
        Vector dpdu, dpdv;
        Normal dndu, dndv;
    };

    Primitive(std::shared_ptr<Geometry> geom, std::shared_ptr<Transform> ltow, std::shared_ptr<Transform> wtol);
    BBox local_bounds() const;
    BBox bounds() const;
    bool intersect(const Ray &ray, float &t, Intersection *i) const;
    friend std::ostream& operator<<(std::ostream &out, const Primitive &primitive);

protected:
    std::shared_ptr<Geometry> _geom;
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
