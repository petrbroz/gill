#include "primitive.h"

namespace gill { namespace core {

using namespace std;

Primitive::Primitive(shared_ptr<Mesh> mesh, shared_ptr<Transform> ltow, shared_ptr<Transform> wtol)
    : _mesh(mesh), _ltow(ltow), _wtol(wtol) { }

BBox Primitive::local_bounds() const {
    return _mesh->bounds();
}

BBox Primitive::bounds() const {
    return (*_ltow)(_mesh->bounds());
}

bool Primitive::intersect(const Ray &ray, float &t, Primitive::Intersection *i) const {
    Ray r = (*_wtol)(ray);
    Mesh::Intersection *mi = nullptr;
    if (i) {
        mi = &i->mi;
    }
    return _mesh->intersect(r, t, mi);
}

}}
