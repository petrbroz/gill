#ifndef GILL_RENDERER_SAMPLED_H_
#define GILL_RENDERER_SAMPLED_H_

#include "core/renderer.h"
#include "core/camera.h"
#include "core/scene.h"
#include "core/sampler.h"

namespace gill { namespace renderer {

using namespace gill::core;

class SampledRenderer : public Renderer {
public:
    SampledRenderer(std::shared_ptr<Camera> camera, std::shared_ptr<SurfaceIntegrator> surface_integrator,
            std::shared_ptr<Sampler> sampler, int thread_tiles[2]);
    virtual void render(const Scene *scene) const override;

protected:
    std::shared_ptr<Sampler> _sampler;
    int _thread_tiles[2];
};

}}

#endif
