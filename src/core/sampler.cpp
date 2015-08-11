#include <cassert>

#include "core/sampler.h"
#include "core/math.h"

namespace gill { namespace core {

using namespace std;

Sampler::Sampler(int x_min, int x_max, int y_min, int y_max, int spp)
    : _x_min(x_min), _x_max(x_max), _y_min(y_min), _y_max(y_max), _spp(spp) {}

Sampler::~Sampler() {}

bool Sampler::report_results(Sample *samples, const Ray *rays, const Spectrum *radiances, int count) {
    return true;
}

void Sampler::compute_subwindow(int h_tiles, int v_tiles, int i, int j, int *x_min, int *x_max, int *y_min, int *y_max) const {
    int w = _x_max - _x_min;
    assert(h_tiles < w);
    int h = _y_max - _y_min;
    assert(v_tiles < h);

    *x_min = floor(lerp<int>((float)i / h_tiles, _x_min, _x_max));
    *x_max = floor(lerp<int>((float)(i + 1) / h_tiles, _x_min, _x_max));
    *y_min = floor(lerp<int>((float)j / v_tiles, _y_min, _y_max));
    *y_max = floor(lerp<int>((float)(j + 1) / v_tiles, _y_min, _y_max));
}

}}
