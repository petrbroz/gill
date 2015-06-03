#include "gtest/gtest.h"
#include "vector.h"

using namespace gill::core;

TEST(VectorTest, Negate) {
    Vector v(1.0, 2.0, 3.0);
    EXPECT_EQ(-v, Vector(-1.0, -2.0, -3.0));
}

TEST(VectorTest, Compare) {
    Vector v(1.0, 2.0, 3.0);
    EXPECT_EQ(v, v);
    EXPECT_NE(v, v * 2.0);
}

TEST(VectorTest, Multiply) {
    float coef = 2.0;
    Vector v(1.0, 2.0, 3.0);
    Vector result(2.0, 4.0, 6.0);
    EXPECT_EQ(v * coef, result);
    v *= coef;
    EXPECT_EQ(v, result);
}

TEST(VectorTest, Divide) {
    float coef = 2.0;
    Vector v(1.0, 2.0, 3.0);
    Vector result(0.5, 1.0, 1.5);
    EXPECT_EQ(v / coef, result);
    v /= coef;
    EXPECT_EQ(v, result);
}

TEST(VectorTest, HasNans) {
    Vector v(1.0, 2.0, 0.0 / 0.0);
    EXPECT_TRUE(has_nans(v));
}

TEST(VectorTest, MinMax) {
    Vector v1(1.0, 5.0, 3.0);
    Vector v2(4.0, 2.0, 6.0);
    EXPECT_EQ(min(v1, v2), Vector(1.0, 2.0, 3.0));
    EXPECT_EQ(max(v1, v2), Vector(4.0, 5.0, 6.0));
}

TEST(VectorTest, Add) {
    Vector v1(1.0, 2.0, 3.0);
    Vector v2(4.0, 5.0, 6.0);
    Point p(7.0, 8.0, 9.0);

    EXPECT_EQ(p + v1, Point(8.0, 10.0, 12.0));
    p += v1;
    EXPECT_EQ(p, Point(8.0, 10.0, 12.0));

    EXPECT_EQ(v1 + v2, Vector(5.0, 7.0, 9.0));
    v1 += v2;
    EXPECT_EQ(v1, Vector(5.0, 7.0, 9.0));
}

TEST(VectorTest, Subtract) {
    Vector v1(1.0, 2.0, 3.0);
    Vector v2(4.0, 5.0, 6.0);
    Point p1(7.0, 8.0, 9.0);
    Point p2(10.0, 11.0, 12.0);

    EXPECT_EQ(p1 - p2, Vector(-3.0, -3.0, -3.0));

    EXPECT_EQ(p1 - v1, Point(6.0, 6.0, 6.0));
    p1 -= v1;
    EXPECT_EQ(p1, Point(6.0, 6.0, 6.0));

    EXPECT_EQ(v1 - v2, Vector(-3.0, -3.0, -3.0));
    v1 -= v2;
    EXPECT_EQ(v1, Vector(-3.0, -3.0, -3.0));
}

TEST(VectorTest, Dot) {
    Vector v1(1.0, 2.0, 3.0);
    Vector v2(4.0, 5.0, 6.0);
    EXPECT_FLOAT_EQ(dot(v1, v2), 32.0);
}

TEST(VectorTest, Cross) {
    Vector v1(1.0, 2.0, 3.0);
    Vector v2(4.0, 5.0, 6.0);
    EXPECT_EQ(cross(v1, v2), Vector(-3.0, 6.0, -3.0));
}

TEST(VectorTest, Length) {
    Vector v(1.0, 2.0, 3.0);
    EXPECT_FLOAT_EQ(length(v), 3.7416575);
}

TEST(VectorTest, Normalize) {
    Vector v(1.0, 2.0, 3.0);
    Vector normalized = normalize(v);
    EXPECT_FLOAT_EQ(length(normalized), 1.0f);
}

TEST(VectorTest, Reflect) {
    Vector eye(1.0, -1.0, 1.0);
    Vector normal(0.0, 1.0, 0.0);
    EXPECT_EQ(reflect(eye, normal), Vector(1.0, 1.0, 1.0));
}

TEST(VectorTest, SphericalCoords) {
    Vector v(1.0, 1.0, 1.0);
    EXPECT_FLOAT_EQ(spherical_phi(v), 0.78539819);
    EXPECT_FLOAT_EQ(spherical_theta(v), 0.0);
}

TEST(VectorTest, Distance) {
    Point p1(1.0, 2.0, 3.0);
    Point p2(4.0, 5.0, 6.0);
    EXPECT_FLOAT_EQ(distance(p1, p2), 5.1961522);
}
