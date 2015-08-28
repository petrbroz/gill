#include "material/glass.h"

namespace gill { namespace material {

using namespace gill::core;

GlassMaterial::GlassMaterial(const Spectrum &kd) : _kd(clamp(kd, 0.f, 1.f)) {
    _bsdf = new BSDF();
    _bsdf->add(new LambertianReflection(_kd));
}

GlassMaterial::~GlassMaterial() {
    delete _bsdf;
}

BSDF * GlassMaterial::bsdf(const Intersection &isec) const {
    return _bsdf;
}

}}
