#include "material/matte.h"

namespace gill { namespace material {

using namespace gill::core;

MatteMaterial::MatteMaterial(const Spectrum &kd) : _kd(clamp(kd, 0.f, 1.f)) {
    _bsdf = new BSDF();
    _bsdf->add(new LambertianReflection(_kd));
}

MatteMaterial::~MatteMaterial() {
    delete _bsdf;
}

BSDF * MatteMaterial::bsdf(const Intersection &isec) const {
    return _bsdf;
}

}}
