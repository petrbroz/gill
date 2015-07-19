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

bool Sphere::intersect(const Ray &ray, float &t, Geometry::Intersection *gi) const {
    Vector rd = ray.d, ro = ray.o;
    float a = dot(rd, rd);
    float b = 2.0 * dot(ro, rd);
    float c = dot(ro, ro) - _radius * _radius;
    float discriminant = b * b - 4 * a * c;
    t = -Infinity;

    if (discriminant > 0.0) {
        float d = sqrt(discriminant);
        float t1 = 0.5 * (-b + d) / a;
        float t2 = 0.5 * (-b - d) / a;
        t = std::min(t1, t2);
    } else if (discriminant == 0.0) {
        t = -0.5 * b / a;
    }

    if (t >= 0.0) {
        if (gi) {
            gi->p = ray(t);
            //gi->n = Normal((gi->p - Point(0.0)) / _radius);
            gi->n = Normal(normalize(gi->p - Point(0.0)));
        }
        return true;
    } else {
        return false;
    }
}

}}
