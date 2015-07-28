#ifndef GILL_CORE_AGGREGATE_H_
#define GILL_CORE_AGGREGATE_H_

#include <vector>

#include "core/kdtree.h"
#include "core/primitive.h"
#include "core/ray.h"

namespace gill { namespace core {

class Aggregate {
public:
    Aggregate(const std::vector<Primitive> &primitives);
    bool intersect(const Ray &ray, float &t, Primitive::Intersection *i) const;

protected:
    std::vector<Primitive> _primitives;
    std::unique_ptr<KdTree<Primitive>> _accelerator;
};

}}

#endif
