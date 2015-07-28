#include "core/aggregate.h"

namespace gill { namespace core {

using namespace std;

Aggregate::Aggregate(const std::vector<Primitive> &primitives) : _primitives(primitives) {
    _accelerator.reset(new KdTree<Primitive>(_primitives, 80.0, 10.0, 8, 32));
}

bool Aggregate::intersect(const Ray &ray, float &t, Primitive::Intersection *i) const {
    t = Infinity;
    return _accelerator->intersect(_primitives, ray, t, i);
}

}}
