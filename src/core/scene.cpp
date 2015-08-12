#include <ctime>
#include <cmath>
#include <thread>

#include "core/scene.h"

namespace gill { namespace core {

/** Computation cost of intersecting a single primitive. */
const float IntersectionCost = 80.0;
/** Computation cost of traversing a node of the accelerator. */
const float TraversalCost = 10.0;
/** Maximum number of entities allowed in a leaf of the accelerator. */
const int MaxGeoms = 8;
/** Maximum allowed depth of the accelerator. */
const int MaxDepth = 32;

using namespace std;

Scene::Scene(const std::vector<Primitive> &primitives) : _primitives(primitives) {
    Primitive * prims = &_primitives[0];
    _accelerator.reset(new KdTree(_primitives.size(),
        IntersectionCost, TraversalCost, MaxGeoms, MaxDepth,
        [prims](uint32_t i) {
            return prims[i].bounds();
        },
        [prims](uint32_t i, const Ray &ray, float &t, Intersection *isec) {
            return prims[i].intersect(ray, t, isec);
        }));
}

bool Scene::intersect(const Ray &ray, float &t, Intersection *isec) const {
    return _accelerator->intersect(ray, t, isec);
}

}}
