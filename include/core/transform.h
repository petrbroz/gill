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
    Matrix _m, _inv;

public:
    Transform() : _m(Identity), _inv(Identity) {}
    Transform(const Matrix& m) : _m(m), _inv(inverse(m)) {}
    Transform(const Matrix& m, const Matrix& inv) : _m(m), _inv(inv) {}
    Transform(const Transform& t) : _m(t._m), _inv(t._inv) {}

    static std::shared_ptr<Transform> translate(const Vector& delta);
    static std::shared_ptr<Transform> translate(float dx, float dy, float dz);
    static std::shared_ptr<Transform> scale(const Vector& coefs);
    static std::shared_ptr<Transform> scale(float sx, float sy, float sz);
    friend bool operator==(const Transform &lhs, const Transform &rhs);
    friend bool operator!=(const Transform &lhs, const Transform &rhs);
    friend Transform inverse(const Transform &t);
    friend Transform operator*(const Transform &lhs, const Transform &rhs);
    friend void operator*=(Transform &lhs, const Transform &rhs);
    friend std::ostream& operator<<(std::ostream &out, const Transform &transform);

    inline Vector operator()(const Vector& v) const {
        return Vector(
            _m.m00 * v.x + _m.m01 * v.y + _m.m02 * v.z,
            _m.m10 * v.x + _m.m11 * v.y + _m.m12 * v.z,
            _m.m20 * v.x + _m.m21 * v.y + _m.m22 * v.z
        );
    }

    inline void operator()(Vector *v) const {
        float x = v->x, y = v->y, z = v->z;
        v->x = _m.m00 * x + _m.m01 * y + _m.m02 * z;
        v->y = _m.m10 * x + _m.m11 * y + _m.m12 * z;
        v->z = _m.m20 * x + _m.m21 * y + _m.m22 * z;
    }

    inline Point operator()(const Point& p) const {
        Point tmp(
            _m.m00 * p.x + _m.m01 * p.y + _m.m02 * p.z + _m.m03,
            _m.m10 * p.x + _m.m11 * p.y + _m.m12 * p.z + _m.m13,
            _m.m20 * p.x + _m.m21 * p.y + _m.m22 * p.z + _m.m23
        );
        float w = _m.m30 * p.x + _m.m31 * p.y + _m.m32 * p.z + _m.m33;
        assert(w != 0.0);
        return tmp / w;
    }

    inline void operator()(Point *p) const {
        float x = p->x, y = p->y, z = p->z;
        p->x = _m.m00 * x + _m.m01 * y + _m.m02 * z + _m.m03;
        p->y = _m.m10 * x + _m.m11 * y + _m.m12 * z + _m.m13;
        p->z = _m.m20 * x + _m.m21 * y + _m.m22 * z + _m.m23;
        float w = _m.m30 * x + _m.m31 * y + _m.m32 * z + _m.m33;
        assert(w != 0.0);
        *p /= w;
    }

    inline Normal operator()(const Normal& n) const {
        return Normal(
            _inv.m00 * n.x + _inv.m10 * n.y + _inv.m20 * n.z,
            _inv.m01 * n.x + _inv.m11 * n.y + _inv.m21 * n.z,
            _inv.m02 * n.x + _inv.m12 * n.y + _inv.m22 * n.z
        );
    }

    inline void operator()(Normal *n) const {
        float x = n->x, y = n->y, z = n->z;
        n->x = _inv.m00 * x + _inv.m10 * y + _inv.m20 * z;
        n->y = _inv.m01 * x + _inv.m11 * y + _inv.m21 * z;
        n->z = _inv.m02 * x + _inv.m12 * y + _inv.m22 * z;
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
    return lhs._m == rhs._m;
}

inline bool operator!=(const Transform &lhs, const Transform &rhs) {
    return lhs._m != rhs._m;
}

inline Transform inverse(const Transform &t) {
    return Transform(t._inv, t._m);
}

inline Transform operator*(const Transform &lhs, const Transform &rhs) {
    return Transform(lhs._m * rhs._m, rhs._inv * lhs._inv);
}

inline void operator*=(Transform &lhs, const Transform &rhs) {
    lhs._m *= rhs._m;
    lhs._inv = rhs._inv * lhs._inv;
}

inline std::ostream& operator<<(std::ostream &out, const Transform &transform) {
    out << transform._m;
    return out;
}

}}

#endif
