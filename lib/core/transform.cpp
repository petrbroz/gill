#include "transform.h"

namespace gill { namespace core {

using namespace std;

Transform Transform::translate(const Vector& delta) {
    return Transform::translate(delta.x, delta.y, delta.z);
}

Transform Transform::translate(float dx, float dy, float dz) {
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

Transform Transform::scale(const Vector& coefs) {
    return Transform::scale(coefs.x, coefs.y, coefs.z);
}

Transform Transform::scale(float sx, float sy, float sz) {
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
