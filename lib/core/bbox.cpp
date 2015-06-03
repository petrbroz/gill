#include "bbox.h"

namespace gill { namespace core {

std::ostream& operator<<(std::ostream &out, const BBox &bbox) {
    out << "BoundingBox(min:" << bbox.min << ",max:" << bbox.max << ")";
    return out;
}

}}
