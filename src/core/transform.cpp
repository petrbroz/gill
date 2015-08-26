#include <cstdarg>
#include "core/transform.h"

namespace gill { namespace core {

using namespace std;

shared_ptr<Transform> Transform::coord_sys(const Vector &right, const Vector &up, const Vector &front) {
    Matrix m(
        right.x, up.x, front.x, 0.f,
        right.y, up.y, front.y, 0.f,
        right.z, up.z, front.z, 0.f,
        0.f, 0.f, 0.f, 1.f
    );
    return make_shared<Transform>(m, inverse(m));
}

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
    Matrix m = Identity;
    m.m00 = axis.x * axis.x + (1.0 - axis.x * axis.x) * c;
    m.m01 = axis.x * axis.y * (1.0 - c) - axis.z * s;
    m.m02 = axis.x * axis.z * (1.0 - c) + axis.y * s;
    m.m10 = axis.x * axis.y * (1.0 - c) + axis.z * s;
    m.m11 = axis.y * axis.y + (1.0 - axis.y * axis.y) * c;
    m.m12 = axis.y * axis.z * (1.0 - c) - axis.x * s;
    m.m20 = axis.x * axis.z * (1.0 - c) - axis.y * s;
    m.m21 = axis.y * axis.z * (1.0 - c) + axis.x * s;
    m.m22 = axis.z * axis.z + (1.0 - axis.z * axis.z) * c;
    return make_shared<Transform>(m, transpose(m));
}

shared_ptr<Transform> Transform::look_at(const Vector &pos, const Vector &target, const Vector &up) {
    Vector dir = normalize(target - pos);
    Vector right = cross(normalize(up), dir);
    if (length_squared(right) == 0) {
        throw std::domain_error("Cannot compute LookAt transform due to colinear dir and up vectors");
    }
    right = normalize(right);
    Vector _up = normalize(cross(dir, right));
    Matrix m = Identity;
    m.m00 = right.x; m.m10 = right.y; m.m20 = right.z;
    m.m01 = _up.x; m.m11 = _up.y; m.m21 = _up.z;
    m.m02 = dir.x; m.m12 = dir.y; m.m22 = dir.z;
    m.m03 = pos.x; m.m13 = pos.y; m.m23 = pos.z;
    //return make_shared<Transform>(inverse(m), m);
    return make_shared<Transform>(m);
}

shared_ptr<Transform> Transform::orthographic(float near, float far) {
    vector<shared_ptr<Transform>> xforms;
    xforms.push_back(translate(0.0, 0.0, -near));
    xforms.push_back(scale(1.0, 1.0, 1.0 / (far - near)));
    return compose(xforms);
}

shared_ptr<Transform> Transform::perspective(float fov, float near, float far) {
    Matrix m = Identity;
    m.m22 = far / (far - near);
    m.m23 = -far * near / (far - near);
    m.m32 = 1.0;
    m.m33 = 0.0;
    float inv_tan = 1.0 / tan(0.5 * radians(fov));
    vector<shared_ptr<Transform>> xforms;
    xforms.push_back(make_shared<Transform>(m));
    xforms.push_back(scale(inv_tan, inv_tan, 1.0));
    return compose(xforms);
}

shared_ptr<Transform> Transform::compose(const vector<shared_ptr<Transform>> &transforms) {
    Matrix m = Identity, inv = Identity;
    for (auto t = transforms.rbegin(); t != transforms.rend(); t++) {
        m *= t->get()->_m;
    }
    for (auto t = transforms.begin(); t != transforms.end(); t++) {
        inv *= t->get()->_inv;
    }
    return make_shared<Transform>(m, inv);
}

}}
