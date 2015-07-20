#ifndef GILL_GEOMETRY_PLANE_H_
#define GILL_GEOMETRY_PLANE_H_

#include <vector>
#include <iostream>

#include "core/bbox.h"
#include "core/ray.h"
#include "geometry/geometry.h"

using namespace gill::core;

namespace gill { namespace geometry {

class Plane : public Geometry {
public:
    BBox bounds() const override;
    bool intersect(const Ray &ray, float &t, Geometry::Intersection *i) const override;
};

}}

#endif
