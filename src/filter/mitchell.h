#ifndef GILL_FILTER_MITCHELL_H_
#define GILL_FILTER_MITCHELL_H_

#include "core/filter.h"

namespace gill { namespace filter {

/**
 * Parametrized image reconstruction filter by Mitchell and Netravali.
 */
class MitchellFilter : public gill::core::Filter {
public:
    MitchellFilter(float width, float height, float b_coef, float c_coef);
    virtual float evaluate(float x, float y) const override;

protected:
    const float _b_coef, _c_coef;
};

}}

#endif
