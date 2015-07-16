#include "transform.h"

namespace gill { namespace core {

using namespace std;

shared_ptr<Transform> Transform::translate(const Vector& delta) {
    return Transform::translate(delta.x, delta.y, delta.z);
}

shared_ptr<Transform> Transform::translate(float dx, float dy, float dz) {
    Matrix m(
        1.0, 0.0, 0.0, dx,
        0.0, 1.0, 0.0, dy,
        0.0, 0.0, 1.0, dz,
        0.0, 0.0, 0.0, 1.0
    );
    Matrix inv(
        1.0, 0.0, 0.0, -dx,
        0.0, 1.0, 0.0, -dy,
        0.0, 0.0, 1.0, -dz,
        0.0, 0.0, 0.0, 1.0
    );
    return make_shared<Transform>(m, inv);
}

shared_ptr<Transform> Transform::scale(const Vector& coefs) {
    return Transform::scale(coefs.x, coefs.y, coefs.z);
}

shared_ptr<Transform> Transform::scale(float sx, float sy, float sz) {
    Matrix m(
        sx, 0.0, 0.0, 0.0,
        0.0, sy, 0.0, 0.0,
        0.0, 0.0, sz, 0.0,
        0.0, 0.0, 0.0, 1.0
    );
    Matrix inv(
        1.0f/sx, 0.0, 0.0, 0.0,
        0.0, 1.0f/sy, 0.0, 0.0,
        0.0, 0.0, 1.0f/sz, 0.0,
        0.0, 0.0, 0.0, 1.0
    );
    return make_shared<Transform>(m, inv);
}

shared_ptr<Transform> Transform::rotate(const Vector& axis, float angle) {
    float s = sin(radians(angle));
    float c = cos(radians(angle));
    Matrix m(0.0);
    m.m00 = axis.x * axis.x + (1.0 - axis.x * axis.x) * c;
    m.m01 = axis.x * axis.y * (1.0 - c) - axis.z * s;
    m.m02 = axis.x * axis.z * (1.0 - c) + axis.y * s;
    m.m10 = axis.x * axis.y * (1.0 - c) + axis.z * s;
    m.m11 = axis.y * axis.y + (1.0 - axis.y * axis.y) * c;
    m.m12 = axis.y * axis.z * (1.0 - c) - axis.x * s;
    m.m20 = axis.x * axis.z * (1.0 - c) - axis.y * s;
    m.m21 = axis.y * axis.z * (1.0 - c) + axis.x * s;
    m.m22 = axis.z * axis.z + (1.0 - axis.z * axis.z) * c;
    m.m33 = 1.0;
    return make_shared<Transform>(m, transpose(m));
}

}}
