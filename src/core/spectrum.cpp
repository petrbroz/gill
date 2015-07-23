#include "core/spectrum.h"
#include "core/math.h"

namespace gill { namespace core {

using namespace std;

SampledSpectrum SampledSpectrum::from_samples(const float *wavelengths, const float *values, int n) {
    SampledSpectrum spectrum;
    resample(wavelengths, values, n, spectrum.c, SpectrumMin, SpectrumMax, SpectrumSamples);
    return spectrum;
}

// Precomputed CIE XYZ spectra
#include "_cie.h"

}}
