#include "core/aggregate.h"

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

Aggregate::Aggregate(const std::vector<Primitive> &primitives) : _primitives(primitives) {
    _accelerator.reset(new KdTree<Primitive>(_primitives, IntersectionCost, TraversalCost, MaxGeoms, MaxDepth));
}

bool Aggregate::intersect(const Ray &ray, float &t, Primitive::Intersection *isec) const {
    return _accelerator->intersect(_primitives, ray, t, isec);
}

}}
