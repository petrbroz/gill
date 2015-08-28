#ifndef GILL_MATERIAL_GLASS_H_
#define GILL_MATERIAL_GLASS_H_

#include "core/material.h"
#include "core/spectrum.h"
#include "core/bsdf.h"

namespace gill { namespace material {

using namespace gill::core;

class GlassMaterial : public Material {
public:
    GlassMaterial(const Spectrum &kd);
    ~GlassMaterial();
    virtual BSDF * bsdf(const Intersection &isec) const override;
    virtual Spectrum _trsm() const override { return _kd; }

protected:
    Spectrum _kd;
    BSDF * _bsdf;
};

}}

#endif
