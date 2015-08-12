#include "core/filter.h"

namespace gill { namespace core {

Filter::Filter(float width, float height)
    : _width(width), _height(height), _inv_width(1.f / width), _inv_height(1.f / height) {}

Filter::~Filter() {}

}}
