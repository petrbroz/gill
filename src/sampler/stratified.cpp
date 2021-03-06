#include "sampler/stratified.h"
#include "core/random.h"

#include <sstream>

namespace gill { namespace sampler {

using namespace gill::core;
using namespace std;

StratifiedSampler::StratifiedSampler(int x_min, int x_max, int y_min, int y_max, int spp)
    : Sampler(x_min, x_max, y_min, y_max), _spp(spp) {
    _x = _x_min;
    _y = _y_min;
}

int StratifiedSampler::max_batch_size() const { return _spp; }

int StratifiedSampler::get_sample_batch(Sample *samples, RNG &rng) {
    if (_y > _y_max) {
        return 0;
    }

    for (int i = 0; i < _spp; ++i) {
        samples[i].image_x = (float)_x + random_float(rng, 0.f, 1.f);
        samples[i].image_y = (float)_y + random_float(rng, 0.f, 1.f);
        samples[i].lens_u = random_float(rng, 0.f, 1.f);
        samples[i].lens_v = random_float(rng, 0.f, 1.f);
    }

    _x++;
    if (_x > _x_max) {
        _x = _x_min;
        _y++;
    }

    return _spp;
}

Sampler * StratifiedSampler::get_subsampler(int h_tiles, int v_tiles, int i, int j) {
    int x0, x1, y0, y1;
    compute_subwindow(h_tiles, v_tiles, i, j, &x0, &x1, &y0, &y1);
    return new StratifiedSampler(x0, x1, y0, y1, _spp);
}

string StratifiedSampler::to_string() const {
    ostringstream desc(ostringstream::ate);
    desc << "stratified (" << _spp << " spp)";
    return desc.str();
}

}}
