#include "gtest/gtest.h"
#include "transform.h"

using namespace gill::core;

TEST(TransformTest, Compare) {
    Transform xform1(Identity, Identity);
    Transform xform2 = Transform::Scale(1.0, 1.0, 1.0);
    EXPECT_EQ(xform1, xform2);
}

TEST(TransformTest, TranslateVector) {
    Vector v(1.0, 2.0, 3.0);
    Transform xform = Transform::Translate(4.0, 5.0, 6.0);
    Vector expected(1.0, 2.0, 3.0);
    EXPECT_EQ(xform(v), expected);
    xform(&v);
    EXPECT_EQ(v, expected);
}

TEST(TransformTest, ScaleVector) {
    Vector v(1.0, 2.0, 3.0);
    Transform xform = Transform::Scale(1.0, 2.0, 3.0);
    Vector expected(1.0, 4.0, 9.0);
    EXPECT_EQ(xform(v), expected);
    xform(&v);
    EXPECT_EQ(v, expected);
}

TEST(TransformTest, TranslatePoint) {
    Point p(1.0, 2.0, 3.0);
    Transform xform = Transform::Translate(4.0, 5.0, 6.0);
    Point expected(5.0, 7.0, 9.0);
    EXPECT_EQ(xform(p), expected);
    xform(&p);
    EXPECT_EQ(p, expected);
}

TEST(TransformTest, ScalePoint) {
    Point p(1.0, 2.0, 3.0);
    Transform xform = Transform::Scale(1.0, 2.0, 3.0);
    Point expected(1.0, 4.0, 9.0);
    EXPECT_EQ(xform(p), expected);
    xform(&p);
    EXPECT_EQ(p, expected);
}

TEST(TransformTest, TranslateNormal) {
    Normal n(1.0, 2.0, 3.0);
    Transform xform = Transform::Translate(4.0, 5.0, 6.0);
    Normal expected(1.0, 2.0, 3.0);
    EXPECT_EQ(xform(n), expected);
    xform(&n);
    EXPECT_EQ(n, expected);
}

TEST(TransformTest, ScaleNormal) {
    Normal n(1.0, 2.0, 3.0);
    Transform xform = Transform::Scale(1.0, 2.0, 3.0);
    Normal expected(1.0, 1.0, 1.0);
    EXPECT_EQ(xform(n), expected);
    xform(&n);
    EXPECT_EQ(n, expected);
}

TEST(TransformTest, TranslateRay) {
    Ray r(Point(1.0, 2.0, 3.0), Vector(4.0, 5.0, 6.0));
    Transform xform = Transform::Translate(4.0, 5.0, 6.0);
    Ray expected(Point(5.0, 7.0, 9.0), Vector(4.0, 5.0, 6.0));
    EXPECT_EQ(xform(r), expected);
}

TEST(TransformTest, ScaleRay) {
    Ray r(Point(1.0, 2.0, 3.0), Vector(4.0, 5.0, 6.0));
    Transform xform = Transform::Scale(1.0, 2.0, 3.0);
    Ray expected(Point(1.0, 4.0, 9.0), Vector(4.0, 10.0, 18.0));
    EXPECT_EQ(xform(r), expected);
}

TEST(TransformTest, TranslateBBox) {
    Transform xform = Transform::Translate(1.0, 2.0, 3.0);
    BBox bb(Point(-1.0), Point(+1.0));
    EXPECT_EQ(xform(bb), BBox(Point(0.0, 1.0, 2.0), Point(2.0, 3.0, 4.0)));
}

TEST(TransformTest, ScaleBBox) {
    Transform xform = Transform::Scale(1.0, 2.0, 3.0);
    BBox bb(Point(-1.0), Point(+1.0));
    EXPECT_EQ(xform(bb), BBox(Point(-1.0, -2.0, -3.0), Point(1.0, 2.0, 3.0)));
}

TEST(TransformTest, Multiply) {
    Transform xform1 = Transform::Translate(-1.0, +1.0, 0.0);
    Transform xform2 = Transform::Scale(2.0, 2.0, 2.0);
    Point p(0.0);
    Point expected(-2.0, 2.0, 0.0);
    EXPECT_EQ((xform2 * xform1)(p), expected);
}

TEST(TransformTest, Inverse) {
    Transform xform1 = Transform::Translate(-1.0, +1.0, 0.0);
    Transform xform2 = Transform::Translate(+1.0, -1.0, 0.0);
    EXPECT_EQ(inverse(xform1), xform2);
}
