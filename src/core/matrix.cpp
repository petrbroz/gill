#include "core/matrix.h"

namespace gill { namespace core {

bool has_nans(const Matrix& m) {
    return std::isnan(m.m00) || std::isnan(m.m01) || std::isnan(m.m02) || std::isnan(m.m03)
        || std::isnan(m.m10) || std::isnan(m.m11) || std::isnan(m.m12) || std::isnan(m.m13)
        || std::isnan(m.m20) || std::isnan(m.m21) || std::isnan(m.m22) || std::isnan(m.m23)
        || std::isnan(m.m30) || std::isnan(m.m31) || std::isnan(m.m32) || std::isnan(m.m33);
}

Matrix inverse(const Matrix& m) {
    Matrix inv(
        +m.m11 * m.m22 * m.m33 - m.m11 * m.m23 * m.m32 - m.m21 * m.m12 * m.m33 + m.m21 * m.m13 * m.m32 + m.m31 * m.m12 * m.m23 - m.m31 * m.m13 * m.m22,
        -m.m10 * m.m22 * m.m33 + m.m10 * m.m23 * m.m32 + m.m20 * m.m12 * m.m33 - m.m20 * m.m13 * m.m32 - m.m30 * m.m12 * m.m23 + m.m30 * m.m13 * m.m22,
        +m.m10 * m.m21 * m.m33 - m.m10 * m.m23 * m.m31 - m.m20 * m.m11 * m.m33 + m.m20 * m.m13 * m.m31 + m.m30 * m.m11 * m.m23 - m.m30 * m.m13 * m.m21,
        -m.m10 * m.m21 * m.m32 + m.m10 * m.m22 * m.m31 + m.m20 * m.m11 * m.m32 - m.m20 * m.m12 * m.m31 - m.m30 * m.m11 * m.m22 + m.m30 * m.m12 * m.m21,
        -m.m01 * m.m22 * m.m33 + m.m01 * m.m23 * m.m32 + m.m21 * m.m02 * m.m33 - m.m21 * m.m03 * m.m32 - m.m31 * m.m02 * m.m23 + m.m31 * m.m03 * m.m22,
        +m.m00 * m.m22 * m.m33 - m.m00 * m.m23 * m.m32 - m.m20 * m.m02 * m.m33 + m.m20 * m.m03 * m.m32 + m.m30 * m.m02 * m.m23 - m.m30 * m.m03 * m.m22,
        -m.m00 * m.m21 * m.m33 + m.m00 * m.m23 * m.m31 + m.m20 * m.m01 * m.m33 - m.m20 * m.m03 * m.m31 - m.m30 * m.m01 * m.m23 + m.m30 * m.m03 * m.m21,
        +m.m00 * m.m21 * m.m32 - m.m00 * m.m22 * m.m31 - m.m20 * m.m01 * m.m32 + m.m20 * m.m02 * m.m31 + m.m30 * m.m01 * m.m22 - m.m30 * m.m02 * m.m21,
        +m.m01 * m.m12 * m.m33 - m.m01 * m.m13 * m.m32 - m.m11 * m.m02 * m.m33 + m.m11 * m.m03 * m.m32 + m.m31 * m.m02 * m.m13 - m.m31 * m.m03 * m.m12,
        -m.m00 * m.m12 * m.m33 + m.m00 * m.m13 * m.m32 + m.m10 * m.m02 * m.m33 - m.m10 * m.m03 * m.m32 - m.m30 * m.m02 * m.m13 + m.m30 * m.m03 * m.m12,
        +m.m00 * m.m11 * m.m33 - m.m00 * m.m13 * m.m31 - m.m10 * m.m01 * m.m33 + m.m10 * m.m03 * m.m31 + m.m30 * m.m01 * m.m13 - m.m30 * m.m03 * m.m11,
        -m.m00 * m.m11 * m.m32 + m.m00 * m.m12 * m.m31 + m.m10 * m.m01 * m.m32 - m.m10 * m.m02 * m.m31 - m.m30 * m.m01 * m.m12 + m.m30 * m.m02 * m.m11,
        -m.m01 * m.m12 * m.m23 + m.m01 * m.m13 * m.m22 + m.m11 * m.m02 * m.m23 - m.m11 * m.m03 * m.m22 - m.m21 * m.m02 * m.m13 + m.m21 * m.m03 * m.m12,
        +m.m00 * m.m12 * m.m23 - m.m00 * m.m13 * m.m22 - m.m10 * m.m02 * m.m23 + m.m10 * m.m03 * m.m22 + m.m20 * m.m02 * m.m13 - m.m20 * m.m03 * m.m12,
        -m.m00 * m.m11 * m.m23 + m.m00 * m.m13 * m.m21 + m.m10 * m.m01 * m.m23 - m.m10 * m.m03 * m.m21 - m.m20 * m.m01 * m.m13 + m.m20 * m.m03 * m.m11,
        +m.m00 * m.m11 * m.m22 - m.m00 * m.m12 * m.m21 - m.m10 * m.m01 * m.m22 + m.m10 * m.m02 * m.m21 + m.m20 * m.m01 * m.m12 - m.m20 * m.m02 * m.m11
    );

    float det = m.m00 * inv.m00 + m.m01 * inv.m10 + m.m02 * inv.m20 + m.m03 * inv.m30;
    if (det == 0.0) {
        throw std::domain_error("Cannot invert singular matrix");
    }

    inv *= 1.0 / det;
    return inv;
}

}}
