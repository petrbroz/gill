#ifndef GILL_CORE_MONTECARLO_H_
#define GILL_CORE_MONTECARLO_H_

#include "core/math.h"
#include "core/vector.h"

namespace gill { namespace core {

/**
 * Converts two random values into a point on hemisphere.
 * @param u1 Random value, uniformly sampled from [0,1) interval.
 * @param u2 Random value, uniformly sampled from [0,1) interval.
 * @returns Random vector, uniformly sampled from unit hemisphere.
 */
inline Vector uniform_hemisphere_sample(float u1, float u2) {
    float r = std::sqrt(std::max(0.f, 1.f - u1 * u1));
    float phi = 2 * Pi * u2;
    return Vector(r * std::cos(phi), r * std::sin(phi), u1);
}

/** Probability distribution function of uniform hemisphere sampling. */
const float UniformHemispherePDF = 0.5f * InvPi;

/**
 * Converts two random values into a point on sphere.
 * @param u1 Random value, uniformly sampled from [0,1) interval.
 * @param u2 Random value, uniformly sampled from [0,1) interval.
 * @returns Random vector, uniformly sampled from unit sphere.
 */
inline Vector uniform_sphere_sample(float u1, float u2) {
    float z = 1.f - 2.f * u1;
    float r = std::sqrt(std::max(0.f, 1.f - z * z));
    float phi = 2 * Pi * u2;
    return Vector(r * std::cos(phi), r * std::sin(phi), z);
}

/** Probability distribution function of uniform sphere sampling. */
const float UniformSpherePDF = 0.25f * InvPi;

/**
 * Converts two random values into a point on disk.
 * @param u1 Random value, uniformly sampled from [0,1) interval.
 * @param u2 Random value, uniformly sampled from [0,1) interval.
 * @param x X-coordinate of the output point.
 * @param y Y-coordinate of the output point.
 */
inline void uniform_disk_sample(float u1, float u2, float& x, float& y) {
    float r = std::sqrt(u1);
    float theta = 2.f * Pi * u2;
    x = r * std::cos(theta);
    y = r * std::sin(theta);
}

/**
 * Converts two random values into a point on disk,
 * avoiding the distorted distribution shown by uniform_disk_sample.
 * @param u1 Random value, uniformly sampled from [0,1) interval.
 * @param u2 Random value, uniformly sampled from [0,1) interval.
 * @param x X-coordinate of the output point.
 * @param y Y-coordinate of the output point.
 */
inline void concentric_disk_sample(float u1, float u2, float& x, float& y) {
    float sx = 2 * u1 - 1;
    float sy = 2 * u2 - 1;

    if (sx == 0.0 && sy == 0.0) {
        x = 0.0; y = 0.0;
        return;
    }

    float r, theta;
    if (sx >= -sy) {
        if (sx > sy) {
            r = sx;
            if (sy > 0.0) {
                theta = sy / r;
            } else {
                theta = 8.f + sy / r;
            }
        } else {
            r = sy;
            theta = 2.f - sx / r;
        }
    } else {
        if (sx <= sy) {
            r = -sx;
            theta = 4.f - sy / r;
        } else {
            r = -sy;
            theta = 6.f + sx / r;
        }
    }
    theta *= 0.25f * M_PI;
    x = r * std::cos(theta);
    y = r * std::sin(theta);
}

/**
 * Converts two random values into a point on cosine-weighted hemisphere
 * by projecting disk-sampled points onto the hemisphere (Malley's method).
 * @param u1 Random value, uniformly sampled from [0,1) interval.
 * @param u2 Random value, uniformly sampled from [0,1) interval.
 * @returns Random vector, uniformly sampled from unit hemisphere.
 */
inline Vector cosine_hemisphere_sample(float u1, float u2) {
    Vector v;
    concentric_disk_sample(u1, u2, v.x, v.y);
    v.z = std::sqrt(std::max(0.f, 1.f - v.x * v.x - v.y * v.y));
    return v;
}

/**
 * Converts two random values into a point on triangle.
 * @param u1 Random value, uniformly sampled from [0,1) interval.
 * @param u2 Random value, uniformly sampled from [0,1) interval.
 * @param x X-coordinate of the output point.
 * @param y Y-coordinate of the output point.
 */
inline void triangle_sample(float u1, float u2, float& x, float& y) {
    float tmp = std::sqrt(u1);
    x = 1.f - tmp;
    y = u2 * tmp;
}

}}

#endif
