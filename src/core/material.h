#ifndef GILL_CORE_MATERIAL_H_
#define GILL_CORE_MATERIAL_H_

#include "core/intersection.h"
#include "core/bsdf.h"

namespace gill { namespace core {

class Material {
public:
    virtual ~Material() {}
    virtual BSDF * bsdf(const Intersection &isec) const = 0;
};

}}

#endif
