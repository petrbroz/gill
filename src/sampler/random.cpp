#include "sampler/random.h"
#include "core/random.h"
#include "core/math.h"

namespace gill { namespace sampler {

using namespace gill::core;

RandomSampler::RandomSampler(int x_min, int x_max, int y_min, int y_max, int num_samples)
    : Sampler(x_min, x_max, y_min, y_max), _num_samples(num_samples), _used_samples(0) {}

int RandomSampler::max_batch_size() const { return 1; }

int RandomSampler::get_sample_batch(Camera::Sample *samples, RNG &rng) {
    if (_num_samples == _used_samples) {
        return 0;
    }

    samples[0].image_x = floor(lerp<float>(random_float(rng, 0.f, 1.f), _x_min, _x_max));
    samples[0].image_y = floor(lerp<float>(random_float(rng, 0.f, 1.f), _y_min, _y_max));
    samples[0].lens_u = random_float(rng, 0.f, 1.f);
    samples[0].lens_v = random_float(rng, 0.f, 1.f);
    _used_samples++;

    return 1;
}

Sampler * RandomSampler::get_subsampler(int h_tiles, int v_tiles, int i, int j) {
    int x0, x1, y0, y1;
    compute_subwindow(h_tiles, v_tiles, i, j, &x0, &x1, &y0, &y1);
    return new RandomSampler(x0, x1, y0, y1, _num_samples / (h_tiles * v_tiles));
}

}}
