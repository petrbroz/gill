#include "gtest/gtest.h"
#include "ray.h"
#include "transform.h"
#include "disk.h"

using namespace gill::core;
using namespace gill::geometry;

TEST(DiskTest, Intersection) {
    Ray ray1(Point(0.0), normalize(Vector(1.0, 1.0, 10.0)));
    Ray ray2(Point(5.0), normalize(Vector(1.0, 1.0, 10.0)));
    const Transform ltow = Transform::Translate(0.0, 0.0, +10.0);
    const Transform wtol = Transform::Translate(0.0, 0.0, -10.0);
    Disk disk(5.0, &ltow, &wtol);
    EXPECT_TRUE(disk.intersect(ray1, nullptr));
    EXPECT_FALSE(disk.intersect(ray2, nullptr));
}
