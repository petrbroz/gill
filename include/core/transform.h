#ifndef GILL_CORE_TRANSFORM_H_
#define GILL_CORE_TRANSFORM_H_

#include <iostream>
#include <cassert>
#include "vector.h"
#include "matrix.h"
#include "ray.h"
#include "bbox.h"

namespace gill { namespace core {

class Transform {
    Matrix m, inv;

public:
    Transform() : m(Identity), inv(Identity) {}
    Transform(const Matrix& _m) : m(_m), inv(inverse(_m)) {}
    Transform(const Matrix& _m, const Matrix& _inv) : m(_m), inv(_inv) {}

    static Transform Translate(const Vector& delta);
    static Transform Translate(float dx, float dy, float dz);
    static Transform Scale(const Vector& coefs);
    static Transform Scale(float sx, float sy, float sz);
    friend bool operator==(const Transform &lhs, const Transform &rhs);
    friend bool operator!=(const Transform &lhs, const Transform &rhs);
    friend Transform inverse(const Transform &t);
    friend Transform operator*(const Transform &lhs, const Transform &rhs);
    friend void operator*=(Transform &lhs, const Transform &rhs);

    inline Vector operator()(const Vector& v) const {
        return Vector(
            m.m00 * v.x + m.m01 * v.y + m.m02 * v.z,
            m.m10 * v.x + m.m11 * v.y + m.m12 * v.z,
            m.m20 * v.x + m.m21 * v.y + m.m22 * v.z
        );
    }

    inline void operator()(Vector *v) const {
        float x = v->x, y = v->y, z = v->z;
        v->x = m.m00 * x + m.m01 * y + m.m02 * z;
        v->y = m.m10 * x + m.m11 * y + m.m12 * z;
        v->z = m.m20 * x + m.m21 * y + m.m22 * z;
    }

    inline Point operator()(const Point& p) const {
        Point tmp(
            m.m00 * p.x + m.m01 * p.y + m.m02 * p.z + m.m03,
            m.m10 * p.x + m.m11 * p.y + m.m12 * p.z + m.m13,
            m.m20 * p.x + m.m21 * p.y + m.m22 * p.z + m.m23
        );
        float w = m.m30 * p.x + m.m31 * p.y + m.m32 * p.z + m.m33;
        assert(w != 0.0);
        return tmp / w;
    }

    inline void operator()(Point *p) const {
        float x = p->x, y = p->y, z = p->z;
        p->x = m.m00 * x + m.m01 * y + m.m02 * z + m.m03;
        p->y = m.m10 * x + m.m11 * y + m.m12 * z + m.m13;
        p->z = m.m20 * x + m.m21 * y + m.m22 * z + m.m23;
        float w = m.m30 * x + m.m31 * y + m.m32 * z + m.m33;
        assert(w != 0.0);
        *p /= w;
    }

    inline Normal operator()(const Normal& n) const {
        return Normal(
            inv.m00 * n.x + inv.m10 * n.y + inv.m20 * n.z,
            inv.m01 * n.x + inv.m11 * n.y + inv.m21 * n.z,
            inv.m02 * n.x + inv.m12 * n.y + inv.m22 * n.z
        );
    }

    inline void operator()(Normal *n) const {
        float x = n->x, y = n->y, z = n->z;
        n->x = inv.m00 * x + inv.m10 * y + inv.m20 * z;
        n->y = inv.m01 * x + inv.m11 * y + inv.m21 * z;
        n->z = inv.m02 * x + inv.m12 * y + inv.m22 * z;
    }

    inline BBox operator()(const BBox &bbox) const {
        const Transform &xform = *this;
        BBox result;
        result += xform(bbox[0]);
        result += xform(bbox[1]);
        result += xform(bbox[2]);
        result += xform(bbox[3]);
        result += xform(bbox[4]);
        result += xform(bbox[5]);
        result += xform(bbox[6]);
        result += xform(bbox[7]);
        return result;
    }

    inline Ray operator()(const Ray& ray) const {
        return Ray((*this)(ray.o), (*this)(ray.d));
    }

    inline bool has_scale() const {
        float x = length_squared((*this)(Vector(1.0, 0.0, 0.0)));
        float y = length_squared((*this)(Vector(0.0, 1.0, 0.0)));
        float z = length_squared((*this)(Vector(0.0, 0.0, 1.0)));
        return x < 0.99f || x > 1.01f || y < 0.99f || y > 1.01f || z < 0.99f || z > 1.01f;
    }
};

inline bool operator==(const Transform &lhs, const Transform &rhs) {
    return lhs.m == rhs.m;
}

inline bool operator!=(const Transform &lhs, const Transform &rhs) {
    return lhs.m != rhs.m;
}

inline Transform inverse(const Transform &t) {
    return Transform(t.inv, t.m);
}

inline Transform operator*(const Transform &lhs, const Transform &rhs) {
    return Transform(lhs.m * rhs.m, rhs.inv * lhs.inv);
}

inline void operator*=(Transform &lhs, const Transform &rhs) {
    lhs.m *= rhs.m;
    lhs.inv = rhs.inv * lhs.inv;
}

}}

#endif
