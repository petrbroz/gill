#ifndef GILL_CORE_SPECTRUM_H_
#define GILL_CORE_SPECTRUM_H_

#include <cassert>
#include <cmath>
#include <iostream>
#include <vector>
#include <algorithm>

namespace gill { namespace core {

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

class SampledSpectrum : public CoefficientSpectrum<SpectrumSamples> {
public:
    static SampledSpectrum from_samples(const float *w, const float *v, int n);
};

class RGB : public CoefficientSpectrum<3> {
};


class XYZ : public CoefficientSpectrum<3> {
};

}}

#endif
