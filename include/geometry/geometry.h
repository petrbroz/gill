#ifndef GILL_GEOMETRY_GEOMETRY_H_
#define GILL_GEOMETRY_GEOMETRY_H_

#include "transform.h"
#include "bbox.h"
#include "ray.h"
#include "vector.h"

namespace gill { namespace geometry {

using namespace gill::core;

struct Hit;

/**
 * Pure geometrical representation of a 3D shape.
 */
class Geometry {
protected:
    /// Transformation from local to world coordinate system
    const Transform *ltow;
    /// Transformation from world to local coordinate system
    const Transform *wtol;
public:
    Geometry(const Transform *localToWorld, const Transform *worldToLocal);
    virtual ~Geometry();
    virtual BBox localBounds() const = 0;
    virtual BBox worldBounds() const;
    virtual bool intersectable() const;
    virtual bool intersect(const Ray &ray, Hit *hit) const = 0;
};

/**
 * Collection of data related to a specific geometry intersection.
 */
struct Hit {
    /// Intersected geometry
    const Geometry *geometry;
    /// Intersection point
    Point p;
    /// Normal at the intersection
    Normal n;
    float t;
    float u, v;
    Vector dpdu, dpdv;
    Normal dndu, dndv;
};

}}

#endif
