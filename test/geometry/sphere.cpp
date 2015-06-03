#include "gtest/gtest.h"
#include "ray.h"
#include "transform.h"
#include "sphere.h"

using namespace gill::core;
using namespace gill::geometry;

TEST(SphereTest, Intersection) {
    Ray ray1(Point(0.0), normalize(Vector(1.0, 1.0, 10.0)));
    Ray ray2(Point(0.0), normalize(Vector(10.0, 10.0, 10.0)));
    const Transform ltow = Transform::Translate(0.0, 0.0, +10.0);
    const Transform wtol = Transform::Translate(0.0, 0.0, -10.0);
    Sphere sphere(5.0, &ltow, &wtol);
    EXPECT_TRUE(sphere.intersect(ray1, nullptr));
    EXPECT_FALSE(sphere.intersect(ray2, nullptr));
}
