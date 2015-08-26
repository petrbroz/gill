#ifndef GILL_INTEGRATOR_PATH_H_
#define GILL_INTEGRATOR_PATH_H_

#include "core/integrator.h"

namespace gill { namespace integrator {

using namespace gill::core;

class PathIntegrator : public SurfaceIntegrator {
public:
    PathIntegrator(int max_depth) {
        _max_depth = max_depth;
    }

    virtual Spectrum Li(const Ray &ray, const Scene *scene, const Sample &sample) const override;

protected:
    int _max_depth;
};

}}

#endif
