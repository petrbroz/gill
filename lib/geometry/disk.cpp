#include "disk.h"

namespace gill { namespace geometry {

Disk::Disk(float _radius, const Transform *localToWorld, const Transform *worldToLocal)
    : Geometry(localToWorld, worldToLocal) {
    radius = _radius;
}

BBox Disk::localBounds() const {
    return BBox(Point(-radius, -radius, 0.0), Point(radius, radius, 0.0));
}

bool Disk::intersect(const Ray &ray, Hit *hit) const {
    Ray r = (*wtol)(ray);
    if (std::abs(r.d.z) < 1e-7) {
        return false;
    }

    float t = -r.o.z / r.d.z;
    Point p = r(t);
    float dist = length(p);
    if (t >= 0.0 && dist < radius) {
        if (hit != nullptr) {
            hit->geometry = this;
            hit->t = t;
            hit->p = (*ltow)(r(t));
            hit->n = normalize((*ltow)(Normal(0.0, 0.0, 1.0)));
        }
        return true;
    } else {
        return false;
    }
}

}}
