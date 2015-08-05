#ifndef GILL_CORE_FRESNEL_H_
#define GILL_CORE_FRESNEL_H_

#include "core/math.h"
#include "core/spectrum.h"

namespace gill { namespace core {

/**
 * Interface for computing Fresnel reflection coefficients, assuming the light is not polarized.
 * @note Fresnel class itself always evaluates to 100% reflection.
 */
class Fresnel {
public:

    /**
     * Evaluate Fresnel reflection.
     * @param cos_i Cosine of the incident angle.
     * @returns Amount of light reflected.
     */
    virtual Spectrum evaluate(float cos_i) const;
};

class FresnelDielectric : public Fresnel {
public:
    FresnelDielectric(float eta_i, float &eta_t) : _eta_i(eta_i), _eta_t(eta_t) {}
    Spectrum evaluate(float cos_i) const override;

protected:
    float _eta_i, _eta_t;
};

class FresnelConductor : public Fresnel {
public:
    FresnelConductor(const Spectrum &eta, const Spectrum &absorp) : _eta(eta), _absorp(absorp) {}
    Spectrum evaluate(float cos_i) const override;

protected:
    Spectrum _eta, _absorp;
};

}}

#endif
