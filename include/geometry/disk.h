#ifndef GILL_GEOMETRY_DISK_H_
#define GILL_GEOMETRY_DISK_H_

#include "geometry.h"

namespace gill { namespace geometry {

class Disk : Geometry {
    float radius;
public:
    Disk(float radius, const Transform *localToWorld, const Transform *worldToLocal);
    BBox localBounds() const override;
    bool intersect(const Ray &ray, Hit *hit) const override;
};

}}

#endif
