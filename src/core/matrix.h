#ifndef GILL_CORE_MATRIX_H_
#define GILL_CORE_MATRIX_H_

#include <cmath>
#include <iostream>

namespace gill { namespace core {

struct Matrix {
    float m00, m01, m02, m03, m10, m11, m12, m13, m20, m21, m22, m23, m30, m31, m32, m33;
    Matrix(float value = 0.0) :
        m00(value), m01(value), m02(value), m03(value),
        m10(value), m11(value), m12(value), m13(value),
        m20(value), m21(value), m22(value), m23(value),
        m30(value), m31(value), m32(value), m33(value) {}
    Matrix(float _m00, float _m01, float _m02, float _m03,
            float _m10, float _m11, float _m12, float _m13,
            float _m20, float _m21, float _m22, float _m23,
            float _m30, float _m31, float _m32, float _m33) :
        m00(_m00), m01(_m01), m02(_m02), m03(_m03),
        m10(_m10), m11(_m11), m12(_m12), m13(_m13),
        m20(_m20), m21(_m21), m22(_m22), m23(_m23),
        m30(_m30), m31(_m31), m32(_m32), m33(_m33) {}
};

const Matrix Identity(
    1.0, 0.0, 0.0, 0.0,
    0.0, 1.0, 0.0, 0.0,
    0.0, 0.0, 1.0, 0.0,
    0.0, 0.0, 0.0, 1.0
);

inline std::ostream& operator<<(std::ostream& out, const Matrix& m) {
    out << "[";
    out << m.m00 << "," << m.m01 << "," << m.m02 << "," << m.m03 << ",";
    out << m.m10 << "," << m.m11 << "," << m.m12 << "," << m.m13 << ",";
    out << m.m20 << "," << m.m21 << "," << m.m22 << "," << m.m23 << ",";
    out << m.m30 << "," << m.m31 << "," << m.m32 << "," << m.m33 << "]";
    return out;
}

bool has_nans(const Matrix& m);
float determinant(const Matrix& m);
Matrix inverse(const Matrix& m);

inline bool operator==(const Matrix& lhs, const Matrix& rhs) {
    return lhs.m00 == rhs.m00 && lhs.m01 == rhs.m01 && lhs.m02 == rhs.m02 && lhs.m03 == rhs.m03
        && lhs.m10 == rhs.m10 && lhs.m11 == rhs.m11 && lhs.m12 == rhs.m12 && lhs.m13 == rhs.m13
        && lhs.m20 == rhs.m20 && lhs.m21 == rhs.m21 && lhs.m22 == rhs.m22 && lhs.m23 == rhs.m23
        && lhs.m30 == rhs.m30 && lhs.m31 == rhs.m31 && lhs.m32 == rhs.m32 && lhs.m33 == rhs.m33;
}

inline bool operator!=(const Matrix& lhs, const Matrix& rhs) {
    return lhs.m00 != rhs.m00 || lhs.m01 != rhs.m01 || lhs.m02 != rhs.m02 || lhs.m03 != rhs.m03
        || lhs.m10 != rhs.m10 || lhs.m11 != rhs.m11 || lhs.m12 != rhs.m12 || lhs.m13 != rhs.m13
        || lhs.m20 != rhs.m20 || lhs.m21 != rhs.m21 || lhs.m22 != rhs.m22 || lhs.m23 != rhs.m23
        || lhs.m30 != rhs.m30 || lhs.m31 != rhs.m31 || lhs.m32 != rhs.m32 || lhs.m33 != rhs.m33;
}

inline Matrix operator*(const Matrix& lhs, const Matrix& rhs) {
    return Matrix(
        lhs.m00 * rhs.m00 + lhs.m01 * rhs.m10 + lhs.m02 * rhs.m20 + lhs.m03 * rhs.m30,
        lhs.m00 * rhs.m01 + lhs.m01 * rhs.m11 + lhs.m02 * rhs.m21 + lhs.m03 * rhs.m31,
        lhs.m00 * rhs.m02 + lhs.m01 * rhs.m12 + lhs.m02 * rhs.m22 + lhs.m03 * rhs.m32,
        lhs.m00 * rhs.m03 + lhs.m01 * rhs.m13 + lhs.m02 * rhs.m23 + lhs.m03 * rhs.m33,
        lhs.m10 * rhs.m00 + lhs.m11 * rhs.m10 + lhs.m12 * rhs.m20 + lhs.m13 * rhs.m30,
        lhs.m10 * rhs.m01 + lhs.m11 * rhs.m11 + lhs.m12 * rhs.m21 + lhs.m13 * rhs.m31,
        lhs.m10 * rhs.m02 + lhs.m11 * rhs.m12 + lhs.m12 * rhs.m22 + lhs.m13 * rhs.m32,
        lhs.m10 * rhs.m03 + lhs.m11 * rhs.m13 + lhs.m12 * rhs.m23 + lhs.m13 * rhs.m33,
        lhs.m20 * rhs.m00 + lhs.m21 * rhs.m10 + lhs.m22 * rhs.m20 + lhs.m23 * rhs.m30,
        lhs.m20 * rhs.m01 + lhs.m21 * rhs.m11 + lhs.m22 * rhs.m21 + lhs.m23 * rhs.m31,
        lhs.m20 * rhs.m02 + lhs.m21 * rhs.m12 + lhs.m22 * rhs.m22 + lhs.m23 * rhs.m32,
        lhs.m20 * rhs.m03 + lhs.m21 * rhs.m13 + lhs.m22 * rhs.m23 + lhs.m23 * rhs.m33,
        lhs.m30 * rhs.m00 + lhs.m31 * rhs.m10 + lhs.m32 * rhs.m20 + lhs.m33 * rhs.m30,
        lhs.m30 * rhs.m01 + lhs.m31 * rhs.m11 + lhs.m32 * rhs.m21 + lhs.m33 * rhs.m31,
        lhs.m30 * rhs.m02 + lhs.m31 * rhs.m12 + lhs.m32 * rhs.m22 + lhs.m33 * rhs.m32,
        lhs.m30 * rhs.m03 + lhs.m31 * rhs.m13 + lhs.m32 * rhs.m23 + lhs.m33 * rhs.m33
    );
}

inline void operator*=(Matrix& lhs, const Matrix& rhs) {
    float tmp[4];
    tmp[0] = lhs.m00 * rhs.m00 + lhs.m01 * rhs.m10 + lhs.m02 * rhs.m20 + lhs.m03 * rhs.m30;
    tmp[1] = lhs.m00 * rhs.m01 + lhs.m01 * rhs.m11 + lhs.m02 * rhs.m21 + lhs.m03 * rhs.m31;
    tmp[2] = lhs.m00 * rhs.m02 + lhs.m01 * rhs.m12 + lhs.m02 * rhs.m22 + lhs.m03 * rhs.m32;
    tmp[3] = lhs.m00 * rhs.m03 + lhs.m01 * rhs.m13 + lhs.m02 * rhs.m23 + lhs.m03 * rhs.m33;
    lhs.m00 = tmp[0]; lhs.m01 = tmp[1]; lhs.m02 = tmp[2]; lhs.m03 = tmp[3];
    tmp[0] = lhs.m10 * rhs.m00 + lhs.m11 * rhs.m10 + lhs.m12 * rhs.m20 + lhs.m13 * rhs.m30;
    tmp[1] = lhs.m10 * rhs.m01 + lhs.m11 * rhs.m11 + lhs.m12 * rhs.m21 + lhs.m13 * rhs.m31;
    tmp[2] = lhs.m10 * rhs.m02 + lhs.m11 * rhs.m12 + lhs.m12 * rhs.m22 + lhs.m13 * rhs.m32;
    tmp[3] = lhs.m10 * rhs.m03 + lhs.m11 * rhs.m13 + lhs.m12 * rhs.m23 + lhs.m13 * rhs.m33;
    lhs.m10 = tmp[0]; lhs.m11 = tmp[1]; lhs.m12 = tmp[2]; lhs.m13 = tmp[3];
    tmp[0] = lhs.m20 * rhs.m00 + lhs.m21 * rhs.m10 + lhs.m22 * rhs.m20 + lhs.m23 * rhs.m30;
    tmp[1] = lhs.m20 * rhs.m01 + lhs.m21 * rhs.m11 + lhs.m22 * rhs.m21 + lhs.m23 * rhs.m31;
    tmp[2] = lhs.m20 * rhs.m02 + lhs.m21 * rhs.m12 + lhs.m22 * rhs.m22 + lhs.m23 * rhs.m32;
    tmp[3] = lhs.m20 * rhs.m03 + lhs.m21 * rhs.m13 + lhs.m22 * rhs.m23 + lhs.m23 * rhs.m33;
    lhs.m20 = tmp[0]; lhs.m21 = tmp[1]; lhs.m22 = tmp[2]; lhs.m23 = tmp[3];
    tmp[0] = lhs.m30 * rhs.m00 + lhs.m31 * rhs.m10 + lhs.m32 * rhs.m20 + lhs.m33 * rhs.m30;
    tmp[1] = lhs.m30 * rhs.m01 + lhs.m31 * rhs.m11 + lhs.m32 * rhs.m21 + lhs.m33 * rhs.m31;
    tmp[2] = lhs.m30 * rhs.m02 + lhs.m31 * rhs.m12 + lhs.m32 * rhs.m22 + lhs.m33 * rhs.m32;
    tmp[3] = lhs.m30 * rhs.m03 + lhs.m31 * rhs.m13 + lhs.m32 * rhs.m23 + lhs.m33 * rhs.m33;
    lhs.m30 = tmp[0]; lhs.m31 = tmp[1]; lhs.m32 = tmp[2]; lhs.m33 = tmp[3];
}

inline Matrix operator*(const Matrix& lhs, float c) {
    return Matrix(
        lhs.m00 * c, lhs.m01 * c, lhs.m02 * c, lhs.m03 * c,
        lhs.m10 * c, lhs.m11 * c, lhs.m12 * c, lhs.m13 * c,
        lhs.m20 * c, lhs.m21 * c, lhs.m22 * c, lhs.m23 * c,
        lhs.m30 * c, lhs.m31 * c, lhs.m32 * c, lhs.m33 * c
    );
}

inline void operator*=(Matrix& lhs, float c) {
    lhs.m00 *= c; lhs.m01 *= c; lhs.m02 *= c; lhs.m03 *= c;
    lhs.m10 *= c; lhs.m11 *= c; lhs.m12 *= c; lhs.m13 *= c;
    lhs.m20 *= c; lhs.m21 *= c; lhs.m22 *= c; lhs.m23 *= c;
    lhs.m30 *= c; lhs.m31 *= c; lhs.m32 *= c; lhs.m33 *= c;
}

inline Matrix transpose(const Matrix& m) {
    return Matrix(
        m.m00, m.m10, m.m20, m.m30,
        m.m01, m.m11, m.m21, m.m31,
        m.m02, m.m12, m.m22, m.m32,
        m.m03, m.m13, m.m23, m.m33
    );
}

}}

#endif
