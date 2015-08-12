#ifndef GILL_FILTER_BOX_H_
#define GILL_FILTER_BOX_H_

#include "core/filter.h"

namespace gill { namespace filter {

/**
 * Simple box filter providing constant weights anywhere in its window.
 */
class BoxFilter : public gill::core::Filter {
public:
    BoxFilter(float width, float height) : Filter(width, height) {}
    virtual float evaluate(float x, float y) const override;
};

}}

#endif
