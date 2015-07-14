#ifndef GILL_CORE_SCENE_H_
#define GILL_CORE_SCENE_H_

#include <iostream>
#include <vector>
#include "bbox.h"
#include "camera.h"
#include "kdtree.h"
#include "primitive.h"
#include "ray.h"

namespace gill { namespace core {

class Scene {
public:
    struct Intersection {
        Primitive::Intersection pi;
    };

    Scene(std::vector<Primitive> primitives, Camera camera);
    BBox bounds() const;
    bool intersect(const Ray &ray, float &t, Intersection *i) const;
    void capture();
    friend std::ostream& operator<<(std::ostream &out, const Scene &scene);

protected:
    std::vector<Primitive> _primitives;
    BBox _bounds;
    std::unique_ptr<KdTree<Primitive>> _accelerator;
    Camera _camera;
};

inline std::ostream& operator<<(std::ostream &out, const Scene &scene) {
    out << "{";
    out << "\"primitives\":[";
    for (auto primitive = scene._primitives.begin(); primitive != scene._primitives.end(); primitive++) {
        if (primitive != scene._primitives.begin()) {
            out << ",";
        }
        out << *primitive;
    }
    out << "],";
    out << "\"camera\":" << scene._camera;
    out << "}";
    return out;
}

}}

#endif
