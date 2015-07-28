#include <ctime>
#include <cmath>
#include <thread>

#include "core/scene.h"

namespace gill { namespace core {

using namespace std;

Scene::Scene(shared_ptr<Aggregate> root, shared_ptr<Camera> camera, shared_ptr<Renderer> renderer)
    : _root(root), _camera(camera), _renderer(renderer) {}

shared_ptr<Aggregate> Scene::root() { return _root; }
shared_ptr<Camera> Scene::camera() { return _camera; }
shared_ptr<Renderer> Scene::renderer() { return _renderer; }

}}
