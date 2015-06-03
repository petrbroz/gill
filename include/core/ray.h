#ifndef GILL_CORE_RAY_H_
#define GILL_CORE_RAY_H_

#include <iostream>
#include "vector.h"

namespace gill { namespace core {

struct Ray {
    Point o;
    Vector d;

    Ray() {}
    Ray(const Point& origin, const Vector& direction) : o(origin), d(direction) {}

    Point operator()(float t) const { return o + d * t; }
};

inline bool operator==(const Ray &lhs, const Ray &rhs) {
    return lhs.o == rhs.o && lhs.d == rhs.d;
}

inline bool operator!=(const Ray &lhs, const Ray &rhs) {
    return lhs.o != rhs.o || lhs.d != rhs.d;
}

inline bool has_nans(const Ray& ray) {
    return has_nans(ray.o) || has_nans(ray.d);
}

std::ostream& operator<<(std::ostream& out, const Ray& ray);

}}

#endif
