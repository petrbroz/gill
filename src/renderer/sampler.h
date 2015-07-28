#ifndef GILL_RENDERER_SAMPLER_H_
#define GILL_RENDERER_SAMPLER_H_

#include "core/renderer.h"
#include "core/aggregate.h"
#include "core/camera.h"

namespace gill { namespace renderer {

using namespace gill::core;

class Sampler : public Renderer {
public:
    Sampler(int samples_per_pixel, int thread_tiles[2]);
    virtual void render(const Aggregate *aggregate, const Camera *camera) const override;

protected:
    int _samples_per_pixel;
    int _thread_tiles[2];
};

}}

#endif
