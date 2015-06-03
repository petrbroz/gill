#include "ray.h"

namespace gill { namespace core {

std::ostream& operator<<(std::ostream& out, const Ray& ray) {
    out << "Ray(origin:" << ray.o << ",direction:" << ray.d << ")";
    return out;
}

}}
