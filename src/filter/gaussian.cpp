#include <cmath>
#include <algorithm>
#include "filter/gaussian.h"

namespace gill { namespace filter {

using namespace std;

inline float gaussian(float alpha, float d, float exp_v) {
    return max(0.f, float(exp(-alpha * d * d) - exp_v));
}

GaussianFilter::GaussianFilter(float width, float height, float alpha)
    : Filter(width, height), _alpha(alpha),
    _exp_x(exp(-alpha * width * width)), _exp_y(exp(-alpha * height * height)) {}

float GaussianFilter::evaluate(float x, float y) const {
    return gaussian(_alpha, x, _exp_x) * gaussian(_alpha, y, _exp_y);
}

}}
