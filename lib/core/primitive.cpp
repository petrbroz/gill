#include "primitive.h"

namespace gill { namespace core {

Primitive::Primitive(const MeshRef mesh, const Transform *ltow, const Transform *wtol)
    : _mesh(mesh), _ltow(ltow), _wtol(wtol) { }

BBox Primitive::local_bounds() const {
    return _mesh->bounds();
}

BBox Primitive::world_bounds() const {
    return (*_ltow)(_mesh->bounds());
}

bool Primitive::intersect(const Ray &ray, float &t, Primitive::Intersection *i) const {
    Ray r = (*_wtol)(ray);
    Mesh::Intersection *mi = nullptr;
    if (i) {
        mi = &i->mesh_isec;
    }
    return _mesh->intersect(r, t, mi);
}

}}
