#include "gtest/gtest.h"
#include "core/ray.h"

using namespace gill::core;

TEST(RayTest, PointOnRay) {
    Ray ray(Point(0.0), Vector(-1.0, 0.0, 1.0));
    EXPECT_EQ(ray(0.5), Point(-0.5, 0.0, 0.5));
}
