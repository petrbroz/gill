#ifndef GILL_CORE_SCENE_H_
#define GILL_CORE_SCENE_H_

#include "core/primitive.h"
#include "core/kdtree.h"
#include "core/ray.h"
#include "core/intersection.h"

namespace gill { namespace core {

class Scene {
public:
    Scene(const std::vector<Primitive> &primitives);

    /**
     * Find closest intersection of ray with any of the contained primitives.
     * @param ray Ray defined in world coordinate system.
     * @param t Parametric distance along the ray to the closest intersection.
     * @param isec Additional intersection data.
     * @note The method will only modify 't' and 'isec' if an intersection closer than 't' was found.
     * @returns True if an intersection was found closer than the current 't'.
     */
    bool intersect(const Ray &ray, float &t, Intersection *isec) const;

    int total_faces() const {
        int total = 0;
        for (auto &p : _primitives) {
            total += p.num_faces();
        }
        return total;
    }

protected:
    std::vector<Primitive> _primitives;
    std::unique_ptr<KdTree> _accelerator;
};

}}

#endif
