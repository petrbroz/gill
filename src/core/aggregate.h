#ifndef GILL_CORE_AGGREGATE_H_
#define GILL_CORE_AGGREGATE_H_

#include <vector>

#include "core/kdtree.h"
#include "core/primitive.h"
#include "core/ray.h"

namespace gill { namespace core {

/**
 * Aggregation of primitives used in a scene.
 */
class Aggregate {
public:
    /**
     * Construct the aggregate and initialize the accelerator.
     */
    Aggregate(const std::vector<Primitive> &primitives);

    /**
     * Find closest intersection of ray with any of the contained primitives.
     * @param ray Ray defined in world coordinate system.
     * @param t Parametric distance along the ray to the closest intersection.
     * @param isec Additional intersection data.
     * @note The method will only modify 't' and 'isec' if an intersection closer than 't' was found.
     * @returns True if an intersection was found closer than the current 't'.
     */
    bool intersect(const Ray &ray, float &t, Intersection *isec) const;

protected:
    std::vector<Primitive> _primitives;
    std::unique_ptr<KdTree> _accelerator;
};

}}

#endif
