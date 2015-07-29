#include "core/primitive.h"

namespace gill { namespace core {

using namespace std;

Primitive::Primitive(shared_ptr<Geometry> geom, shared_ptr<Transform> ltow, shared_ptr<Transform> wtol)
    : _geom(geom), _ltow(ltow), _wtol(wtol) { }

BBox Primitive::local_bounds() const {
    return _geom->bounds();
}

BBox Primitive::bounds() const {
    return (*_ltow)(_geom->bounds());
}

bool Primitive::intersect(const Ray &ray, float &t, Primitive::Intersection *isec) const {
    Ray r = (*_wtol)(ray);
    Geometry::Intersection *gi = nullptr;
    if (isec) {
        gi = &isec->gi;
    }
    bool hit = _geom->intersect(r, t, gi);
    if (hit && gi) {
        gi->p = (*_ltow)(gi->p);
        gi->n = (*_ltow)(gi->n);
    }
    return hit;
}

}}
