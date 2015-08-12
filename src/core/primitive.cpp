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
    Ray local_ray = (*_wtol)(ray);
    float local_t = Infinity;
    if (t < local_t) {
        Point local_hit = (*_wtol)(ray(t));
        local_t = distance(local_hit, local_ray.o) / length(local_ray.d);
    }

    bool hit = _geom->intersect(local_ray, local_t, isec);
    if (hit && isec) {
        isec->p = (*_ltow)(isec->p);
        isec->n = (*_ltow)(isec->n);
        t = distance(isec->p, ray.o) / length(ray.d);
    }
    return hit;
}

}}
