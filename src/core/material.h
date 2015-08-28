#ifndef GILL_CORE_MATERIAL_H_
#define GILL_CORE_MATERIAL_H_

#include "core/intersection.h"
#include "core/spectrum.h"
#include "core/bsdf.h"

namespace gill { namespace core {

class Material {
public:
    virtual ~Material() {}
    virtual BSDF * bsdf(const Intersection &isec) const = 0;
    virtual Spectrum _emit() const { return Spectrum(0.f); }
    virtual Spectrum _diff() const { return Spectrum(0.f); }
    virtual Spectrum _refl() const { return Spectrum(0.f); }
    virtual Spectrum _trsm() const { return Spectrum(0.f); }
};

}}

#endif
