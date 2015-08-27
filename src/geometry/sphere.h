#ifndef GILL_GEOMETRY_SPHERE_H_
#define GILL_GEOMETRY_SPHERE_H_

#include <vector>
#include <iostream>

#include "core/bbox.h"
#include "core/geometry.h"
#include "core/ray.h"
#include "core/vector.h"

using namespace gill::core;

namespace gill { namespace geometry {

/**
 * Unit sphere.
 */
class Sphere : public Geometry {
public:
    Sphere(float radius) : _radius(radius) {}
    BBox bounds() const override;
    bool intersect(const Ray &ray, float &t, Intersection *i) const override;
    int num_faces() const { return 1; }

protected:
    float _radius;
};

}}

#endif
