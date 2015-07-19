#ifndef GILL_GEOMETRY_GEOMETRY_H_
#define GILL_GEOMETRY_GEOMETRY_H_

#include <vector>
#include <iostream>

#include "core/vector.h"
#include "core/ray.h"
#include "core/bbox.h"

using namespace gill::core;

namespace gill { namespace geometry {

class Geometry {
public:
    struct Intersection {
        float t;
        float u, v;
        Vector p;
        Normal n;
    };

    virtual BBox bounds() const = 0;
    virtual bool intersect(const Ray &ray, float &t, Intersection *i) const = 0;
};

}}

#endif
