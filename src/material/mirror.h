#ifndef GILL_MATERIAL_MIRROR_H_
#define GILL_MATERIAL_MIRROR_H_

#include "core/material.h"
#include "core/spectrum.h"
#include "core/bsdf.h"

namespace gill { namespace material {

using namespace gill::core;

class MirrorMaterial : public Material {
public:
    MirrorMaterial(const Spectrum &kd);
    ~MirrorMaterial();
    virtual BSDF * bsdf(const Intersection &isec) const override;
    virtual Spectrum _refl() const override { return _kd; }

protected:
    Spectrum _kd;
    BSDF * _bsdf;
};

}}

#endif
