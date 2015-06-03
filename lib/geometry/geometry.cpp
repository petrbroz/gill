#include "geometry.h"

namespace gill { namespace geometry {

Geometry::Geometry(const Transform *localToWorld, const Transform *worldToLocal)
    : ltow(localToWorld), wtol(worldToLocal) {
}

Geometry::~Geometry() {
}

BBox Geometry::worldBounds() const {
    return (*ltow)(localBounds());
}

bool Geometry::intersectable() const {
    return true;
}

}}
