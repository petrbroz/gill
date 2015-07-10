#include "scene.h"

namespace gill { namespace core {

using namespace std;

Scene::Scene(vector<Primitive> primitives) : _primitives(primitives) {
    _accelerator.reset(new KdTree<Primitive>(_primitives, 80.0, 10.0, 8, 32));
}

BBox Scene::bounds() const {
    return BBox();
}

bool Scene::intersect(const Ray &ray, float &t, Scene::Intersection *i) const {
    return _accelerator->intersect(_primitives, ray, t, nullptr);
}

}}
