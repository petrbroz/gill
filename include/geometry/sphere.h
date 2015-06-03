#ifndef GILL_GEOMETRY_SPHERE_H_
#define GILL_GEOMETRY_SPHERE_H_

#include "geometry.h"

namespace gill { namespace geometry {

class Sphere : Geometry {
    float radius;
public:
    Sphere(float radius, const Transform *localToWorld, const Transform *worldToLocal);
    BBox localBounds() const override;
    bool intersect(const Ray &ray, Hit *hit) const override;
};

}}

#endif
