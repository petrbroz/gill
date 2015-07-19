#include "gtest/gtest.h"
#include "core/bbox.h"

using namespace gill::core;

TEST(BBoxTest, Compare) {
    BBox bb1(Point(-1.0), Point(+1.0));
    BBox bb2(Point(+1.0), Point(-1.0));
    EXPECT_EQ(bb1, bb2);
    bb1.max = Point(1.0, 2.0, 3.0);
    EXPECT_NE(bb1, bb2);
}

TEST(BBoxTest, Add) {
    BBox bb1(Point(-1.0), Point(0.0));
    BBox bb2(Point(0.0), Point(+1.0));
    EXPECT_EQ(bb1 + Point(0.0), bb1);
    EXPECT_EQ(bb1 + Point(2.0), BBox(Point(-1.0), Point(+2.0)));
    EXPECT_EQ(bb1 + bb2, BBox(Point(-1.0), Point(+1.0)));
    bb1 += bb2;
    EXPECT_EQ(bb1, BBox(Point(-1.0), Point(+1.0)));
}

TEST(BBoxTest, Surface) {
    BBox bb(Point(-1.0), Point(+1.0));
    EXPECT_FLOAT_EQ(surface(bb), 24.0);
}

TEST(BBoxTest, Volume) {
    BBox bb(Point(-1.0), Point(+1.0));
    EXPECT_FLOAT_EQ(volume(bb), 8.0);
}

TEST(BBoxTest, Corners) {
    BBox bb(Point(-1.0), Point(+1.0));
    EXPECT_EQ(bb[0], Point(-1.0, -1.0, -1.0));
    EXPECT_EQ(bb[1], Point(+1.0, -1.0, -1.0));
    EXPECT_EQ(bb[2], Point(-1.0, +1.0, -1.0));
    EXPECT_EQ(bb[3], Point(+1.0, +1.0, -1.0));
    EXPECT_EQ(bb[4], Point(-1.0, -1.0, +1.0));
    EXPECT_EQ(bb[5], Point(+1.0, -1.0, +1.0));
    EXPECT_EQ(bb[6], Point(-1.0, +1.0, +1.0));
    EXPECT_EQ(bb[7], Point(+1.0, +1.0, +1.0));
}
