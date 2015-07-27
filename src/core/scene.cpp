#include <ctime>
#include <cmath>
#include <thread>

#include "core/scene.h"

namespace gill { namespace core {

using namespace std;

Scene::Scene(std::vector<Primitive> primitives, shared_ptr<Camera> camera)
        : _primitives(primitives), _camera(camera) {
    _accelerator.reset(new KdTree<Primitive>(_primitives, 80.0, 10.0, 8, 32));
}

BBox Scene::bounds() const {
    return BBox();
}

const int samples = 32;

void capture_fragment(const Scene *scene, const Camera *camera, int minx, int miny, int maxx, int maxy) {
    float t;
    Scene::Intersection si;
    for (int y = miny; y <= maxy; ++y) {
        for (int x = minx; x <= maxx; ++x) {
            float r = 0.0, g = 0.0, b = 0.0;
            for (int s = 0; s < samples; ++s) {
                float u = 2.0 * rand() / RAND_MAX - 1.0;
                float v = 2.0 * rand() / RAND_MAX - 1.0;
                Camera::Sample sample = {(float)x, (float)y, u, v};
                Ray ray = camera->generate_ray(sample);
                if (scene->intersect(ray, t, &si)) {
                    Normal n = normalize(si.pi.gi.n);
                    r += n.x * 0.5 + 0.5;
                    g += n.y * 0.5 + 0.5;
                    b += n.z * 0.5 + 0.5;
                }
            }
            camera->_film->set_sample(x, y, Spectrum(r / samples, g / samples, b / samples));
        }
    }
}

void Scene::capture() {
    int res_x = _camera->_film->_xres;
    int res_y = _camera->_film->_yres;
    auto begin_time = clock();
    thread t1(capture_fragment, this, _camera.get(), 0, 0, 255, 255);
    thread t2(capture_fragment, this, _camera.get(), 256, 0, 511, 255);
    thread t3(capture_fragment, this, _camera.get(), 0, 256, 255, 511);
    thread t4(capture_fragment, this, _camera.get(), 256, 256, 511, 511);
    t1.join(); t2.join(); t3.join(); t4.join();
    auto end_time = clock();
    _camera->_film->print_ppm();
    cerr << "resolution:[" << res_x << "," << res_y << "]" << endl;
    cerr << "samples_per_pixel:" << samples << endl;
    cerr << "render_time:" << float(end_time - begin_time) / CLOCKS_PER_SEC << "s" << endl;
}

bool Scene::intersect(const Ray &ray, float &t, Scene::Intersection *si) const {
    Primitive::Intersection *pi = nullptr;
    if (si) {
        pi = &(si->pi);
    }
    t = Infinity;
    return _accelerator->intersect(_primitives, ray, t, pi);
}

}}
