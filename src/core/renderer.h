#ifndef GILL_CORE_RENDERER_H_
#define GILL_CORE_RENDERER_H_

#include "core/scene.h"
#include "core/camera.h"

namespace gill { namespace core {

/**
 * Abstraction of an algorithm for rendering gill::core::Scene through gill::core::Camera.
 */
class Renderer {
public:
    Renderer(std::shared_ptr<Camera> camera) : _camera(camera) {}
    virtual void render(const Scene *scene) const = 0;

protected:
    std::shared_ptr<Camera> _camera;
};

}}

#endif
