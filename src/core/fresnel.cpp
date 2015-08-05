#include <cmath>
#include "core/fresnel.h"

namespace gill { namespace core {

using namespace std;

Spectrum Fresnel::evaluate(float cos_i) const {
    return Spectrum(1.f);
}

/**
 * Compute Fresnel reflection for dielectric media.
 * @param cos_i Cosine of the incident angle.
 * @param cos_t Cosine of the transmission angle.
 * @param eta_i Index-of-refraction (per spectral component) of the incident media.
 * @param eta_t Index-of-refraction (per spectral component) of the transmission media.
 */
Spectrum fresnel_diel(float cos_i, float cos_t, const Spectrum &eta_i, const Spectrum &eta_t) {
    Spectrum parl_refl = (eta_t * cos_i - eta_i * cos_t) / (eta_t * cos_i + eta_i * cos_t);
    Spectrum perp_refl = (eta_i * cos_i - eta_t * cos_t) / (eta_i * cos_i + eta_t * cos_t);
    return (parl_refl * parl_refl + perp_refl + perp_refl) * 0.5f;
}

Spectrum FresnelDielectric::evaluate(float cos_i) const {
    cos_i = clamp(cos_i, -1.f, +1.f);
    float ei = _eta_i, et = _eta_t;
    if (cos_i > 0.f) {
        swap(ei, et);
    }
    float sin_t = ei/et * sqrt(max(0.f, 1.f - cos_i * cos_i)); // Snell's law
    if (sin_t >= 1.f) {
        return 1.f;
    } else {
        float cos_t = sqrt(max(0.f, 1.f - sin_t * sin_t));
        return fresnel_diel(abs(cos_i), cos_t, ei, et);
    }
}

/**
 * Compute Fresnel reflection for conductors.
 * @param cos_i Cosine of the incident angle.
 * @param eta_t Index-of-refraction (per spectral component) of the conductor;
 * @param absorp Conductor absorption coefficient.
 */
Spectrum fresnel_cond(float cos_i, const Spectrum &eta, const Spectrum &absorp) {
    Spectrum ec = eta * cos_i * 2.f;
    Spectrum tmp = (eta * eta + absorp * absorp) * cos_i * cos_i;
    Spectrum parl_refl_sq = (tmp - ec + 1.f) / (tmp + ec + 1.f);
    tmp = eta * eta + absorp * absorp;
    Spectrum perp_refl_sq = (tmp - ec + cos_i * cos_i) / (tmp + ec + cos_i * cos_i);
    return (parl_refl_sq + perp_refl_sq) * 0.5f;
}

Spectrum FresnelConductor::evaluate(float cos_i) const {
    return fresnel_cond(abs(cos_i), _eta, _absorp);
}

}}
