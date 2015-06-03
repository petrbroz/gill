#include "gtest/gtest.h"
#include "matrix.h"

using namespace gill::core;

TEST(MatrixTest, Compare) {
    Matrix m1 = Identity, m2 = Identity;
    EXPECT_EQ(m1, m2);
    m1.m33 = 0.0;
    EXPECT_NE(m1, m2);
}

TEST(MatrixTest, MultiplyScalar) {
    float c = 0.5;
    Matrix m(
        1.0, 0.0, 0.0, 0.0,
        0.0, 2.0, 0.0, 0.0,
        0.0, 0.0, 3.0, 0.0,
        0.0, 0.0, 0.0, 4.0
    );
    Matrix expected(
        0.5, 0.0, 0.0, 0.0,
        0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 1.5, 0.0,
        0.0, 0.0, 0.0, 2.0
    );
    EXPECT_EQ(m * c, expected);
    m *= c;
    EXPECT_EQ(m, expected);
}

TEST(MatrixTest, MultiplyMatrix) {
    Matrix m1(
        1.0, 0.0, 0.0, 0.0,
        0.0, 2.0, 0.0, 0.0,
        0.0, 0.0, 3.0, 0.0,
        0.0, 0.0, 0.0, 4.0
    );
    Matrix m2(
        5.0, 0.0, 0.0, 0.0,
        0.0, 6.0, 0.0, 0.0,
        0.0, 0.0, 7.0, 0.0,
        0.0, 0.0, 0.0, 8.0
    );
    Matrix expected(
        5.0, 0.0, 0.0, 0.0,
        0.0, 12.0, 0.0, 0.0,
        0.0, 0.0, 21.0, 0.0,
        0.0, 0.0, 0.0, 32.0
    );
    EXPECT_EQ(m1 * m2, expected);
    m1 *= m2;
    EXPECT_EQ(m1, expected);
}

TEST(MatrixTest, Transpose) {
    Matrix m(
        0, 1, 2, 3,
        4, 5, 6, 7,
        8, 9, 10, 11,
        12, 13, 14, 15
    );
    Matrix expected(
        0, 4, 8, 12,
        1, 5, 9, 13,
        2, 6, 10, 14,
        3, 7, 11, 15
    );
    EXPECT_EQ(transpose(m), expected);
}

TEST(MatrixTest, Inverse) {
    Matrix m = Identity;
    m.m00 = m.m11 = m.m22 = 2.0;
    Matrix expected = Identity;
    expected.m00 = expected.m11 = expected.m22 = 0.5;
    EXPECT_EQ(inverse(m), expected);
}
