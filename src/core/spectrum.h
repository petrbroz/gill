#ifndef GILL_CORE_SPECTRUM_H_
#define GILL_CORE_SPECTRUM_H_

#include <cassert>
#include <cmath>
#include <iostream>
#include <vector>
#include <algorithm>

namespace gill { namespace core {

/**
 * Generic definition of a wavelength spectrum distribution with predefined number of discrete coefficients.
 */
template <int num_coefs>
class CoefficientSpectrum {
public:
    CoefficientSpectrum(float v = 0.0) {
        for (int i = 0; i < num_coefs; ++i) {
            c[i] = v;
        }
    }

    inline bool operator==(const CoefficientSpectrum &rhs) const {
        for (int i = 0; i < num_coefs; ++i) {
            if (c[i] != rhs.c[i]) {
                return false;
            }
        }
        return true;
    }

    inline bool operator!=(const CoefficientSpectrum &rhs) const {
        return !(*this == rhs);
    }

    inline CoefficientSpectrum &operator+=(const CoefficientSpectrum &rhs) {
        for (int i = 0; i < num_coefs; ++i) { c[i] += rhs.c[i]; }
        return *this;
    }

    inline CoefficientSpectrum operator+(const CoefficientSpectrum &rhs) const {
        CoefficientSpectrum result = *this;
        return result += rhs;
    }

    inline CoefficientSpectrum &operator-=(const CoefficientSpectrum &rhs) {
        for (int i = 0; i < num_coefs; ++i) { c[i] -= rhs.c[i]; }
        return *this;
    }

    inline CoefficientSpectrum operator-(const CoefficientSpectrum &rhs) const {
        CoefficientSpectrum result = *this;
        return result -= rhs;
    }

    inline CoefficientSpectrum &operator*=(float rhs) {
        for (int i = 0; i < num_coefs; ++i) { c[i] *= rhs; }
        return *this;
    }

    inline CoefficientSpectrum operator*(float rhs) const {
        CoefficientSpectrum result = *this;
        return result *= rhs;
    }

    inline CoefficientSpectrum &operator/=(float rhs) {
        for (int i = 0; i < num_coefs; ++i) { c[i] /= rhs; }
        return *this;
    }

    inline CoefficientSpectrum operator/(float rhs) const {
        CoefficientSpectrum result = *this;
        return result /= rhs;
    }

    inline CoefficientSpectrum &operator*=(const CoefficientSpectrum &rhs) {
        for (int i = 0; i < num_coefs; ++i) { c[i] *= rhs.c[i]; }
        return *this;
    }

    inline CoefficientSpectrum operator*(const CoefficientSpectrum &rhs) const {
        CoefficientSpectrum result = *this;
        return result *= rhs;
    }

    inline CoefficientSpectrum &operator/=(const CoefficientSpectrum &rhs) {
        for (int i = 0; i < num_coefs; ++i) { c[i] /= rhs.c[i]; }
        return *this;
    }

    inline CoefficientSpectrum operator/(const CoefficientSpectrum &rhs) const {
        CoefficientSpectrum result = *this;
        return result /= rhs;
    }

    inline float& operator[](int i) {
        assert(i >= 0 && i < num_coefs);
        return c[i];
    }

    inline float operator[](int i) const {
        assert(i >= 0 && i < num_coefs);
        return c[i];
    }

    friend bool is_black(const CoefficientSpectrum &spectrum) {
        for (int i = 0; i < num_coefs; ++i) {
            if (spectrum.c[i] != 0.0) {
                return false;
            }
        }
        return true;
    }

    friend CoefficientSpectrum clamp(const CoefficientSpectrum &spectrum, float min, float max) {
        CoefficientSpectrum result;
        for (int i = 0; i < num_coefs; ++i) {
            result.c[i] = std::min(std::max(spectrum.c[i], min), max);
        }
        return result;
    }

    friend CoefficientSpectrum sqrt(const CoefficientSpectrum &spectrum) {
        CoefficientSpectrum result;
        for (int i = 0; i < num_coefs; ++i) {
            result.c[i] = std::sqrt(spectrum.c[i]);
        }
        return result;
    }

    friend CoefficientSpectrum pow(const CoefficientSpectrum &spectrum, float e) {
        CoefficientSpectrum result;
        for (int i = 0; i < num_coefs; ++i) {
            result.c[i] = std::pow(spectrum.c[i], e);
        }
        return result;
    }

protected:
    float c[num_coefs];
};

const float SpectrumMin = SPECTRUM_MIN;
const float SpectrumMax = SPECTRUM_MAX;
const int SpectrumSamples = SPECTRUM_RES;

/**
 * Spectral curve defined by a number of Y-values for evenly distributed lambdas.
 */
class SampledSpectrum : public CoefficientSpectrum<SpectrumSamples> {
public:
    static SampledSpectrum from_samples(const float *w, const float *v, int n);
};

inline void xyz_to_rgb(const float xyz[3], float rgb[3]) {
    rgb[0] = +3.240479 * xyz[0] - 1.537150 * xyz[1] - 0.498535 * xyz[2];
    rgb[1] = -0.969256 * xyz[0] + 1.875991 * xyz[1] + 0.041556 * xyz[2];
    rgb[2] = +0.055648 * xyz[0] - 0.204043 * xyz[1] + 1.057311 * xyz[2];
}

inline void rgb_to_xyz(const float rgb[3], float xyz[3]) {
    xyz[0] = +0.412453 * rgb[0] + 0.357580 * rgb[1] + 0.180423 * rgb[2];
    xyz[1] = +0.212671 * rgb[0] + 0.715160 * rgb[1] + 0.072169 * rgb[2];
    xyz[2] = +0.019334 * rgb[0] + 0.119193 * rgb[1] + 0.950227 * rgb[2];
}

class RGB : public CoefficientSpectrum<3> {
public:
    RGB(float r = 0.0, float g = 0.0, float b = 0.0) {
        c[0] = r; c[1] = g; c[2] = b;
    }

    RGB(const CoefficientSpectrum<3> &s) {
        c[0] = s[0]; c[1] = s[1]; c[2] = s[2];
    }
};


class XYZ : public CoefficientSpectrum<3> {
};

typedef RGB Spectrum;

}}

#endif
