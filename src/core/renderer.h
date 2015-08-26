#ifndef GILL_CORE_RENDERER_H_
#define GILL_CORE_RENDERER_H_

#include "core/scene.h"
#include "core/camera.h"
#include "core/integrator.h"

namespace gill { namespace core {

/**
 * Abstraction of an algorithm for rendering gill::core::Scene through gill::core::Camera.
 */
class Renderer {
public:
    Renderer(std::shared_ptr<Camera> camera, std::shared_ptr<SurfaceIntegrator> surface_integrator)
        : _camera(camera), _surface_integrator(surface_integrator) {}
    virtual void render(const Scene *scene) const = 0;

protected:
    std::shared_ptr<Camera> _camera;
    std::shared_ptr<SurfaceIntegrator> _surface_integrator;
};

}}

#endif
