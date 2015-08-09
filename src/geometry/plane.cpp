#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <vector>
#include <ctime>

#include "geometry/plane.h"

namespace gill { namespace geometry {

using namespace std;

BBox Plane::bounds() const {
    return BBox(Point(-0.5, -0.5, 0.0), Point(0.5, 0.5, 0.0));
}

bool Plane::intersect(const Ray &ray, float &t, Intersection *isec) const {
    float oz = ray.o.z, dz = ray.d.z;
    if (dz == 0.0) {
        return false;
    }

    float _t = -oz / dz;
    Point _p = ray(_t);
    if (_t >= 0.0 && _t < t && abs(_p.x) <= 0.5 && abs(_p.y) <= 0.5) {
        t = _t;
        if (isec) {
            isec->p = _p;
            isec->n = Normal(0.0, 0.0, oz > 0.0 ? 1.0 : -1.0);
        }
        return true;
    } else {
        return false;
    }
}

}}
