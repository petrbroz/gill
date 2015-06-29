#include "primitive.h"

namespace gill { namespace core {

Primitive::Primitive(const Mesh *_mesh, const Transform *localToWorld, const Transform *worldToLocal)
    : mesh(_mesh), ltow(localToWorld), wtol(worldToLocal) { }

BBox Primitive::localBounds() const {
    return mesh->bounds();
}

BBox Primitive::worldBounds() const {
    return (*ltow)(mesh->bounds());
}

bool Primitive::intersect(const Ray &ray, float &t, Primitive::Intersection *i) const {
    Ray r = (*wtol)(ray);
    Mesh::Intersection *_mi = nullptr;
    if (i) {
        _mi = &i->mesh_isec;
    }
    return mesh->intersect(r, t, _mi);
}

}}
