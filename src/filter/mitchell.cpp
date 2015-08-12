#include <cmath>
#include "filter/mitchell.h"

namespace gill { namespace filter {

using namespace std;

inline float mitchell(float x, float b, float c) {
    x = abs(2.f * x);
    if (x > 1.f) {
        return (x * x * x * (-b - 6 * c)
                + x * x * (6 * b + 30 * c)
                + x * (-12 * b - 48 * c)
                + (8 * b + 24 * c)) / 6.f;
    } else {
        return (x * x * x * (12 - 9 * b - 6 * c)
                + x * x * (-18 + 12 * b + 6 * c)
                + (6 - 2 * b)) / 6.f;
    }
}

MitchellFilter::MitchellFilter(float width, float height, float b_coef, float c_coef)
    : Filter(width, height), _b_coef(b_coef), _c_coef(c_coef) {}

float MitchellFilter::evaluate(float x, float y) const {
    return mitchell(x * _inv_width, _b_coef, _c_coef) * mitchell(y * _inv_height, _b_coef, _c_coef);
}

}}
