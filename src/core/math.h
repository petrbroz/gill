#ifndef GILL_CORE_MATH_H_
#define GILL_CORE_MATH_H_

#include <cmath>
#include <limits>

namespace gill { namespace core {

const float Infinity = std::numeric_limits<float>::infinity();

inline bool almost_zero(float v) {
    return std::abs(v) < 1e-8;
}

inline float radians(float degrees) {
    return degrees * 3.14159265 / 180.0;
}

template<typename T>
inline T lerp(float t, T v0, T v1) {
    return v0 + t * (v1 - v0);
}

/**
 * Resamples the input of arbitrarily distributed (x,y) pairs
 * into an evenly distributed output with given range and number of samples.
 * @param xi Array of input X-values.
 * @param yi Array of input Y-values.
 * @param ni Number of the input (x,y) pairs.
 * @param yo Preallocated array for the output Y-values.
 * @param min_xo Minimum of the output X-range.
 * @param max_xo Maximum of the output X-range.
 * @param no Number of the output (x,y) pairs.
 */
void resample(const float *xi, const float *yi, int ni, float *yo, float min_xo, float max_xo, int no);

}}

#endif
