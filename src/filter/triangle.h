#ifndef GILL_FILTER_TRIANGLE_H_
#define GILL_FILTER_TRIANGLE_H_

#include "core/filter.h"

namespace gill { namespace filter {

/**
 * Simple triangle filter with weights approaching 0 towards the edges of the window.
 */
class TriangleFilter : public gill::core::Filter {
public:
    TriangleFilter(float width, float height) : Filter(width, height) {}
    virtual float evaluate(float x, float y) const override;
};

}}

#endif
