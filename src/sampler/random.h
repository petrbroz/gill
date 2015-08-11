#ifndef GILL_SAMPLER_RANDOM_H_
#define GILL_SAMPLER_RANDOM_H_

#include <iostream>

#include "core/sampler.h"

namespace gill { namespace sampler {

class RandomSampler : public gill::core::Sampler {
public:
    RandomSampler(int x_min, int x_max, int y_min, int y_max, int spp);
    virtual int max_batch_size() const override;
    virtual int get_sample_batch(gill::core::Sample *samples, gill::core::RNG &rng) override;
    virtual gill::core::Sampler * get_subsampler(int h_tiles, int v_tiles, int i, int j) override;

protected:
    int _x, _y;
};

}}

#endif
