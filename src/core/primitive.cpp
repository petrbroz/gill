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

bool Primitive::intersect(const Ray &ray, float &t, Intersection *isec) const {
    Ray r = (*_wtol)(ray);
    bool hit = _geom->intersect(r, t, isec);
    if (hit && isec) {
        isec->p = (*_ltow)(isec->p);
        isec->n = (*_ltow)(isec->n);
    }
    return hit;
}

}}
