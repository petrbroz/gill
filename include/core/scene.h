#ifndef GILL_CORE_SCENE_H_
#define GILL_CORE_SCENE_H_

#include <vector>
#include "bbox.h"
#include "ray.h"
#include "kdtree.h"
#include "primitive.h"

namespace gill { namespace core {

class Scene {
public:
    struct Intersection {
        Primitive::Intersection pi;
    };

    Scene(std::vector<Primitive> primitives);
    BBox bounds() const;
    bool intersect(const Ray &ray, float &t, Intersection *i) const;

protected:
    std::vector<Primitive> _primitives;
    BBox _bounds;
    std::unique_ptr<KdTree<Primitive>> _accelerator;
};

}}

#endif
