#ifndef GILL_MATERIAL_MATTE_H_
#define GILL_MATERIAL_MATTE_H_

#include "core/material.h"
#include "core/spectrum.h"
#include "core/bsdf.h"

namespace gill { namespace material {

using namespace gill::core;

class MatteMaterial : public Material {
public:
    MatteMaterial(const Spectrum &kd);
    ~MatteMaterial();
    virtual BSDF * bsdf(const Intersection &isec) const override;

protected:
    Spectrum _kd;
    BSDF * _bsdf;
};

}}

#endif
