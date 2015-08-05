#ifndef GILL_CORE_BSDF_H_
#define GILL_CORE_BSDF_H_

#include "core/fresnel.h"
#include "core/math.h"
#include "core/spectrum.h"
#include "core/vector.h"

namespace gill { namespace core {

// Helpers for spherical coordinates (theta and phi angles) of vectors in the shading coordinate system.
inline float cos_theta(const Vector &w) { return w.z; }
inline float abs_cos_theta(const Vector &w) { return std::abs(cos_theta(w.z)); }
inline float squared_sin_theta(const Vector &w) { return std::max(0.f, 1.f - cos_theta(w) * cos_theta(w)); }
inline float sin_theta(const Vector &w) { return std::sqrt(squared_sin_theta(w)); }
inline float cos_phi(const Vector &w) {
    float sst = squared_sin_theta(w);
    return sst > 0.f ? clamp(w.x / std::sqrt(sst), -1.f, +1.f) : 1.f;
}
inline float sin_phi(const Vector &w) {
    float sst = squared_sin_theta(w);
    return sst > 0.f ? clamp(w.y / std::sqrt(sst), -1.f, +1.f) : 0.f;
}

/**
 * Abstraction of either a bidirectional reflectance or a bidirectional transmittance distribution function.
 *
 * @note By convention, incident (wi) and outgoing (wo) vectors are assumed to be normalized and outward facing.
 */
class BxDF {
public:
    enum class Type {
        Reflection          = 1,
        Transmission        = 1 << 1,
        Diffuse             = 1 << 2,
        Glossy              = 1 << 3,
        Specular            = 1 << 4,
        AllTypes            = Diffuse | Glossy | Specular,
        AllReflections      = Reflection | AllTypes,
        AllTransmissions    = Transmission | AllTypes,
        All                 = AllReflections | AllTransmissions
    };

    const Type type;

    BxDF(Type t) : type(t) {}
    virtual ~BxDF() {}

    /**
     * Compute reflectance or transmittance for given incident and outgoing light direction.
     * @param wo Outgoing light direction.
     * @param wi Incident light direction.
     * @returns Reflectance or transmittance spectrum.
     * @note Both wo and wi are expected to be oriented away from the surface.
     */
    virtual Spectrum f(const Vector &wo, const Vector &wi) const = 0;

    /**
     * Compute reflectance or transmittance similarly to BxDF::f, for delta distributions
     * and for randomly sampled directions.
     * @param wo Outgoing light direction.
     * @param wi Incident light direction.
     * @note u1, u2 and pdf will be used for Monte Carlo integration.
     */
    //virtual Spectrum sample_f(const Vector &wo, const Vector &wi, float u1, float u2, float *pdf) const;

    /**
     * Compute the 'hemispherical-directional' reflectance.
     * @param wo Outgoing light direction.
     * @note num_samples and samples will be used for Monte Carlo integration.
     */
    //virtual Spectrum rho(const Vector &wo, int num_samples, const float *samples) const;

    /**
     * Compute the 'hemispherical-hemispherical' reflectance.
     * @note num_samples, samples1 and samples2 will be used for Monte Carlo integration.
     */
    //virtual Spectrum rho(int num_samples, const float *samples1, const float *samples2) const;

    //virtual float pdf(const Vector &wo, const Vector &wi) const;
};

/**
 * Helper class representing a BTDF based on an existing BRDF or vice versa.
 */
class BRDFuncToBTDFunc : public BxDF {
public:
    BRDFuncToBTDFunc(BxDF *bxdf);
    Spectrum f(const Vector &wo, const Vector &wi) const override;

protected:
    BxDF *_bxdf;
};

/**
 * Helper class representing a BxDF with scaled Spectrum distributions.
 */
class ScaledBxDF : public BxDF {
public:
    ScaledBxDF(BxDF *bxdf, const Spectrum &scale);
    Spectrum f(const Vector &wo, const Vector &wi) const override;

protected:
    BxDF *_bxdf;
    Spectrum _scale;
};

/**
 * Bidirectional scattering distribution function, defined by one or more gill::core::BxDF functions.
 */
class BSDF {
public:

    /**
     * Reflectance or transmittance for given pair of vectors.
     * @param wi Incident light direction.
     * @param wo Outgoing light direction.
     * @returns Reflectance or transmittance spectrum.
     * @note Both wo and wi are expected to be oriented away from the surface.
     */
    virtual Spectrum f(const Vector &wi, const Vector &wo) const = 0;
};

}}

#endif
