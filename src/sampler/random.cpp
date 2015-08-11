#include "sampler/random.h"
#include "core/random.h"

namespace gill { namespace sampler {

using namespace gill::core;

RandomSampler::RandomSampler(int x_min, int x_max, int y_min, int y_max, int spp)
    : Sampler(x_min, x_max, y_min, y_max, spp) {
    _x = _x_min;
    _y = _y_min;
}

int RandomSampler::max_batch_size() const { return _spp; }

int RandomSampler::get_sample_batch(Camera::Sample *samples, RNG &rng) {
    if (_y > _y_max) {
        return 0;
    }

    for (int i = 0; i < _spp; ++i) {
        samples[i].image_x = _x + random<float>(rng, 0.f, 1.f);
        samples[i].image_y = _y + random<float>(rng, 0.f, 1.f);
        samples[i].lens_u = random<float>(rng, 0.f, 1.f);
        samples[i].lens_v = random<float>(rng, 0.f, 1.f);
    }

    _x++;
    if (_x > _x_max) {
        _x = _x_min;
        _y++;
    }

    return _spp;
}

Sampler * RandomSampler::get_subsampler(int h_tiles, int v_tiles, int i, int j) {
    int x0, x1, y0, y1;
    compute_subwindow(h_tiles, v_tiles, i, j, &x0, &x1, &y0, &y1);
    return new RandomSampler(x0, x1, y0, y1, _spp);
}

}}
