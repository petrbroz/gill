#include "vector.h"

namespace gill { namespace core {

std::ostream& operator<<(std::ostream& out, const Vector& v) {
    out << "Vector(" << v.x << "," << v.y << "," << v.z << ")";
    return out;
}

std::ostream& operator<<(std::ostream& out, const Point& p) {
    out << "Point(" << p.x << "," << p.y << "," << p.z << ")";
    return out;
}

std::ostream& operator<<(std::ostream& out, const Normal& n) {
    out << "Normal(" << n.x << "," << n.y << "," << n.z << ")";
    return out;
}

}}
