#ifndef GILL_CORE_SAMPLER_H_
#define GILL_CORE_SAMPLER_H_

#include <iostream>

#include "core/ray.h"
#include "core/spectrum.h"
#include "core/intersection.h"

namespace gill { namespace core {

struct Sample {
    float img_x, img_y;
    float lens_u, lens_v;
};

/**
 * Abstraction of an algorithm generating image samples to be processed by the renderer.
 */
class Sampler {
public:
    /**
     * Initializes the sampler for a given segment of the output image (in pixels).
     * @param x_min Minimum X-extent of the image segment.
     * @param x_max Maximum X-extent of the image segment.
     * @param y_min Minimum Y-extent of the image segment.
     * @param y_max Maximum Y-extent of the image segment.
     * @param spp Samples-per-pixel.
     */
    Sampler(int x_min, int x_max, int y_min, int y_max, int spp);
    virtual ~Sampler();

    virtual int max_batch_size() const = 0;
    virtual int get_sample_batch(Sample *samples) = 0;
    virtual bool report_results(Sample *samples, const Ray *rays, const Spectrum *radiances, int count) = 0;
    virtual Sampler * get_subsampler(int index, int total) = 0;

protected:
    int _x_min, _x_max, _y_min, _y_max, _spp;
};

}}

#endif
