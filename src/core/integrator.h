#ifndef GILL_CORE_INTEGRATOR_H_
#define GILL_CORE_INTEGRATOR_H_

#include <iostream>

#include "core/spectrum.h"
#include "core/ray.h"
#include "core/scene.h"
#include "core/sampler.h"

namespace gill { namespace core {

class SurfaceIntegrator {
public:
    virtual ~SurfaceIntegrator() {};
    virtual Spectrum Li(const Ray &ray, const Scene *scene, const Sample &sample) const = 0;
};

}}

#endif
