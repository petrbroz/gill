#include "material/emissive.h"

namespace gill { namespace material {

using namespace gill::core;

EmissiveMaterial::EmissiveMaterial(const Spectrum &emissive)
    : _emissive(clamp(emissive, 0.f, 1.f)) {
}

BSDF * EmissiveMaterial::bsdf(const Intersection &isec) const {
    return nullptr;
}

}}
