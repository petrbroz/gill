#ifndef GILL_FILTER_GAUSSIAN_H_
#define GILL_FILTER_GAUSSIAN_H_

#include "core/filter.h"

namespace gill { namespace filter {

/**
 * Image reconstruction filter with Gaussian distribution of weights.
 */
class GaussianFilter : public gill::core::Filter {
public:
    GaussianFilter(float width, float height, float alpha);
    virtual float evaluate(float x, float y) const override;

protected:
    const float _alpha, _exp_x, _exp_y;
};

}}

#endif
