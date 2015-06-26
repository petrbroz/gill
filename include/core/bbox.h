#ifndef GILL_CORE_BBOX_H_
#define GILL_CORE_BBOX_H_

#include <iostream>
#include <limits>
#include <cassert>
#include "vector.h"
#include "ray.h"

namespace gill { namespace core {

const float Infinity = std::numeric_limits<float>::infinity();

struct BBox {
    Point min, max;

    BBox() : min(Point(Infinity)), max(Point(-Infinity)) {}
    BBox(const Point &p) : min(p), max(p) {}
    BBox(const Point &p1, const Point &p2) {
        min = gill::core::min(p1, p2);
        max = gill::core::max(p1, p2);
    }

    bool contains(const Point &p) const {
        return p.x >= min.x && p.y >= min.y && p.z >= min.z
            && p.x <= max.x && p.y <= max.y && p.z <= max.z;
    }

    bool intersects(const Ray &r, float &tmin, float &tmax) const {
        tmin = 0.0;
        tmax = +Infinity;
        for (int i = 0; i < 3; i++) {
            float ro = r.o[i], rd = r.d[i];
            float bmin = min[i], bmax = max[i];
            if (almost_zero(rd)) {
                if (ro < bmin || ro > bmax) {
                    return false;
                } else {
                    continue;
                }
            }

            float inv_rd = 1.0 / rd;
            float t1 = (bmin - ro) * inv_rd;
            float t2 = (bmax - ro) * inv_rd;
            if (t1 > t2) {
                std::swap(t1, t2);
            }
            if (t1 > tmin) {
                tmin = t1;
            }
            if (t2 < tmax) {
                tmax = t2;
            }
            if (tmin > tmax) {
                return false;
            }
        }

        return true;
    }

    void expand(float delta) {
        min -= Vector(delta);
        max += Vector(delta);
    }

    Point operator[](int i) const {
        assert(i >= 0 && i < 8);
        return Point((i & 1) != 0 ? max.x : min.x, (i & 2) != 0 ? max.y : min.y, (i & 4) != 0 ? max.z : min.z);
    }
};

inline bool operator==(const BBox &lhs, const BBox &rhs) {
    return lhs.min == rhs.min && lhs.max == rhs.max;
}

inline bool operator!=(const BBox &lhs, const BBox &rhs) {
    return lhs.min != rhs.min || lhs.max != rhs.max;
}

inline BBox operator+(const BBox &b1, const BBox &b2) {
    return BBox(min(b1.min, b2.min), max(b1.max, b2.max));
}

inline void operator+=(BBox &b1, const BBox &b2) {
    b1.min = min(b1.min, b2.min);
    b1.max = max(b1.max, b2.max);
}

inline BBox operator+(const BBox &b, const Point &p) {
    return BBox(min(b.min, p), max(b.max, p));
}

inline void operator+=(BBox &b, const Point &p) {
    b.min = min(b.min, p);
    b.max = max(b.max, p);
}

inline float surface(const BBox &b) {
    Vector d = b.max - b.min;
    return 2.0 * (d.x * d.y + d.x * d.z + d.y * d.z);
}

inline float volume(const BBox &b) {
    Vector d = b.max - b.min;
    return d.x * d.y * d.z;
}

std::ostream& operator<<(std::ostream &out, const BBox &bbox);

}}

#endif
