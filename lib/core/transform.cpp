#include "transform.h"

namespace gill { namespace core {

Transform Transform::Translate(const Vector& delta) {
    return Transform::Translate(delta.x, delta.y, delta.z);
}

Transform Transform::Translate(float dx, float dy, float dz) {
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
    return Transform(m, inv);
}

Transform Transform::Scale(const Vector& coefs) {
    return Transform::Scale(coefs.x, coefs.y, coefs.z);
}

Transform Transform::Scale(float sx, float sy, float sz) {
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
    return Transform(m, inv);
}

}}
