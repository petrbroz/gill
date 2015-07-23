#ifndef GILL_CORE_MATH_H_
#define GILL_CORE_MATH_H_

#include <cmath>

namespace gill { namespace core {

inline bool almost_zero(float v) {
    return std::abs(v) < 1e-8;
}

inline float radians(float degrees) {
    return degrees * 3.14159265 / 180.0;
}

//inline float clamp(float value, float min, float max) {
//    return std::min(std::max(value, min), max);
//}

inline float lerp(float v0, float v1, float t) {
    return v0 + t * (v1 - v0);
}

void resample(const float *xi, const float *yi, int ni, float *yo, float min_xo, float max_xo, int no);

}}

#endif
