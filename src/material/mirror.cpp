#include "material/mirror.h"

namespace gill { namespace material {

using namespace gill::core;

MirrorMaterial::MirrorMaterial(const Spectrum &kd) : _kd(clamp(kd, 0.f, 1.f)) {
    _bsdf = new BSDF();
    _bsdf->add(new LambertianReflection(_kd));
}

MirrorMaterial::~MirrorMaterial() {
    delete _bsdf;
}

BSDF * MirrorMaterial::bsdf(const Intersection &isec) const {
    return _bsdf;
}

}}
