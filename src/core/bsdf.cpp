#include "core/bsdf.h"

namespace gill { namespace core {

using namespace std;

BRDFuncToBTDFunc::BRDFuncToBTDFunc(BxDF *bxdf)
    : BxDF(bxdf->type == BxDF::Type::Reflection ? BxDF::Type::Transmission : BxDF::Type::Reflection) {
    _bxdf = bxdf;
}

Spectrum BRDFuncToBTDFunc::f(const Vector &wo, const Vector &wi) const {
    return _bxdf->f(wo, Vector(wi.x, wi.y, -wi.z));
}

ScaledBxDF::ScaledBxDF(BxDF *bxdf, const Spectrum &scale) : BxDF(bxdf->type), _bxdf(bxdf), _scale(scale) { }

Spectrum ScaledBxDF::f(const Vector &wo, const Vector &wi) const {
    return _scale * _bxdf->f(wo, wi);
}

}}
