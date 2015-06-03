#include "sphere.h"

namespace gill { namespace geometry {

Sphere::Sphere(float _radius, const Transform *localToWorld, const Transform *worldToLocal)
    : Geometry(localToWorld, worldToLocal) {
    radius = _radius;
}

BBox Sphere::localBounds() const {
    return BBox(Point(-radius), Point(radius));
}

bool Sphere::intersect(const Ray &ray, Hit *hit) const {
    Ray r = (*wtol)(ray);
    Vector o = r.o;
    float a = dot(r.d, r.d);
    float b = 2.0 * dot(o, r.d);
    float c = dot(o, o) - radius * radius;
    float d = b * b - 4.0 * a * c;
    if (d >= 0.0) {
        float t = (d > 0.0) ? std::min(-b-std::sqrt(d), -b+std::sqrt(d)) : -b;
        t /= 2.0 * a;
        if (hit != nullptr && t >= 0.0) {
            hit->geometry = this;
            hit->t = t;
            Point p = r(t);
            Normal n = p;
            hit->n = normalize((*ltow)(n));
            hit->p = (*ltow)(p);
        }
        return t >= 0.0;
    } else {
        return false;
    }
}

}}
