#ifndef GILL_CORE_INTERSECTION_H_
#define GILL_CORE_INTERSECTION_H_

#include "core/vector.h"
#include "core/spectrum.h"

namespace gill { namespace core {

/**
 * Collection of data related to a specific primitive intersection.
 */
struct Intersection {
    Point p;
    Normal n;
    float u, v;
    Vector dpdu, dpdv;
    Normal dndu, dndv;
    Spectrum emit, refl;
};

}}

#endif
