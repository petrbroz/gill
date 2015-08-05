#include "core/bsdf.h"

namespace gill { namespace core {

using namespace std;

Spectrum BxDF::sample_f(const Vector &wo, Vector &wi, float u1, float u2, float *_pdf) const {
    wi = normalize(Vector(u1, u2, 1.f)); // should use Monte Carlo eventually
    if (wo.z < 0.f) {
        wi.z *= -1.f;
    }
    *_pdf = pdf(wo, wi);
    return f(wo, wi);
}

float BxDF::pdf(const Vector &wo, const Vector &wi) const {
    return same_hemisphere(wo, wi) ? abs_cos_theta(wi) * InvPi : 0.f;
}


inline Vector swap_z(const Vector &v) { return Vector(v.x, v.y, -v.z); }

BRDFuncToBTDFunc::BRDFuncToBTDFunc(BxDF *bxdf)
    : BxDF(bxdf->type == BxDF::Type::Reflection ? BxDF::Type::Transmission : BxDF::Type::Reflection) {
    _bxdf = bxdf;
}

Spectrum BRDFuncToBTDFunc::f(const Vector &wo, const Vector &wi) const {
    return _bxdf->f(wo, swap_z(wi));
}

Spectrum BRDFuncToBTDFunc::sample_f(const Vector &wo, Vector &wi, float u1, float u2, float *_pdf) const {
    Spectrum f = _bxdf->sample_f(wo, wi, u1, u2, _pdf);
    wi = swap_z(wi);
    return f;
}

float BRDFuncToBTDFunc::pdf(const Vector &wo, const Vector &wi) const {
    return _bxdf->pdf(wo, swap_z(wi));
}


ScaledBxDF::ScaledBxDF(BxDF *bxdf, const Spectrum &scale) : BxDF(bxdf->type), _bxdf(bxdf), _scale(scale) { }

Spectrum ScaledBxDF::f(const Vector &wo, const Vector &wi) const {
    return _scale * _bxdf->f(wo, wi);
}

Spectrum ScaledBxDF::sample_f(const Vector &wo, Vector &wi, float u1, float u2, float *_pdf) const {
    Spectrum f = _bxdf->sample_f(wo, wi, u1, u2, _pdf);
    return _scale * f;
}


SpecularReflection::SpecularReflection(Fresnel *fresnel, const Spectrum &r)
    : BxDF(BxDF::Type::Reflection | BxDF::Type::Specular), _fresnel(fresnel), _r(r) { }

Spectrum SpecularReflection::f(const Vector &wo, const Vector &wi) const {
    return Spectrum(0.f);
}

Spectrum SpecularReflection::sample_f(const Vector &wo, Vector &wi, float u1, float u2, float *_pdf) const {
    wi = Vector(-wo.x, -wo.y, wo.z);
    *_pdf = 1.f;
    return _fresnel->evaluate(cos_theta(wo)) * _r / abs_cos_theta(wi);
}

float SpecularReflection::pdf(const Vector &wo, const Vector &wi) const {
    return 0.f;
}


SpecularTransmission::SpecularTransmission(const Spectrum &t, float eta_i, float eta_t)
    : BxDF(BxDF::Type::Transmission | BxDF::Type::Specular), _t(t),
    _eta_i(eta_i), _eta_t(eta_t), _fresnel(eta_i, eta_t) { }

Spectrum SpecularTransmission::f(const Vector &wo, const Vector &wi) const {
    return Spectrum(0.f);
}

Spectrum SpecularTransmission::sample_f(const Vector &wo, Vector &wi, float u1, float u2, float *_pdf) const {
    bool entering = cos_theta(wo) > 0.f;
    float ei = _eta_i, et = _eta_t;
    if (!entering) {
        swap(ei, et);
    }
    float sin2_i = squared_sin_theta(wo);
    float eta = ei / et;
    float sin2_t = eta * eta * sin2_i;
    if (sin2_t >= 1.f) {
        return 0.f;
    }
    float cos_t = sqrt(std::max(0.f, 1.f - sin2_t));
    if (entering) {
        cos_t = -cos_t;
    }
    wi = Vector(eta * -wo.x, eta * -wo.y, cos_t);
    *_pdf = 1.f;
    Spectrum fr = _fresnel.evaluate(cos_theta(wo));
    return (Spectrum(1.f) - fr) * _t * (et * et) / (ei * ei) / abs_cos_theta(wi);
}

float SpecularTransmission::pdf(const Vector &wo, const Vector &wi) const {
    return 0.f;
}

}}
