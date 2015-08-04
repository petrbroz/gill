#ifndef GILL_CORE_SCENE_H_
#define GILL_CORE_SCENE_H_

#include "core/aggregate.h"
#include "core/camera.h"
#include "core/renderer.h"

namespace gill { namespace core {

/**
 * Top-level entity encapsulating the objects to render (gill::core::Aggregate),
 * the camera to generate rays from, and the renderer for computing the output.
 */
class Scene {
public:
    Scene(std::shared_ptr<Aggregate> root, std::shared_ptr<Camera> camera, std::shared_ptr<Renderer> renderer);
    std::shared_ptr<Aggregate> root();
    std::shared_ptr<Camera> camera();
    std::shared_ptr<Renderer> renderer();

protected:
    std::shared_ptr<Aggregate> _root;
    std::shared_ptr<Camera> _camera;
    std::shared_ptr<Renderer> _renderer;
};

}}

#endif
