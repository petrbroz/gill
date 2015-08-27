#ifndef GILL_CORE_GEOMETRY_H_
#define GILL_CORE_GEOMETRY_H_

#include <vector>
#include <iostream>

#include "core/vector.h"
#include "core/ray.h"
#include "core/bbox.h"
#include "core/intersection.h"

namespace gill { namespace core {

/**
 * Abstraction of a geometry, able to define its bounds and compute ray intersections.
 */
class Geometry {
public:
    virtual BBox bounds() const = 0;
    virtual bool intersect(const Ray &ray, float &t, Intersection *i) const = 0;
    virtual int num_faces() const = 0;
};

}}

#endif
