#ifndef GILL_MATERIAL_EMISSIVE_H_
#define GILL_MATERIAL_EMISSIVE_H_

#include "core/material.h"
#include "core/spectrum.h"
#include "core/bsdf.h"

namespace gill { namespace material {

using namespace gill::core;

class EmissiveMaterial : public Material {
public:
    EmissiveMaterial(const Spectrum &emissive);

    //TODO: what should a light return?
    virtual BSDF * bsdf(const Intersection &isec) const override;
    virtual Spectrum _emit() const override { return _emissive; }

protected:
    Spectrum _emissive;
};

}}

#endif
