#include <cmath>
#include <algorithm>
#include "filter/triangle.h"

namespace gill { namespace filter {

using namespace std;

float TriangleFilter::evaluate(float x, float y) const {
    return max(0.f, _width - abs(x)) * max(0.f, _height - abs(y));
}

}}
