#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <vector>
#include <ctime>

#include "geometry/sphere.h"

namespace gill { namespace geometry {

using namespace std;

BBox Sphere::bounds() const {
    return BBox(Point(-_radius), Point(_radius));
}

bool Sphere::intersect(const Ray &ray, float &t, Intersection *isec) const {
    Vector rd = ray.d, ro = ray.o;
    float a = dot(rd, rd);
    float b = 2.0 * dot(ro, rd);
    float c = dot(ro, ro) - _radius * _radius;
    float discriminant = b * b - 4 * a * c;
    float _t = +Infinity;

    if (discriminant > 0.0) {
        float d = sqrt(discriminant);
        float t1 = 0.5 * (-b + d) / a;
        float t2 = 0.5 * (-b - d) / a;
        _t = std::min(t1, t2);
    } else if (discriminant == 0.0) {
        _t = -0.5 * b / a;
    }

    if (_t >= 0.0 && _t < t) {
        t = _t;
        if (isec) {
            isec->p = ray(t);
            isec->n = Normal((isec->p - Point(0.0)) / _radius);
        }
        return true;
    } else {
        return false;
    }
}

}}
