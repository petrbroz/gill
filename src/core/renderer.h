#ifndef GILL_CORE_RENDERER_H_
#define GILL_CORE_RENDERER_H_

#include "core/aggregate.h"
#include "core/camera.h"

namespace gill { namespace core {

class Renderer {
public:
    virtual void render(const Aggregate *aggregate, const Camera *camera) const = 0;
};

}}

#endif
