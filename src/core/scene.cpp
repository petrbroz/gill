#include <ctime>
#include <cmath>

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

void Scene::capture() {
    auto begin_time = clock();
    float t;
    Scene::Intersection si;

    int res_x = _camera->_film->_xres;
    int res_y = _camera->_film->_yres;

    const int samples = 64;

    cout << "P3" << endl;
    cout << res_x << " " << res_y << endl;
    cout << "255" << endl;
    for (int y = 0; y < res_y; ++y) {
        for (int x = 0; x < res_x; ++x) {
            int r = 0, g = 0, b = 0;
            for (int s = 0; s < samples; ++s) {
                float u = 2.0 * rand() / RAND_MAX - 1.0;
                float v = 2.0 * rand() / RAND_MAX - 1.0;
                Camera::Sample sample = {(float)x, (float)(res_y - y), u, v};
                Ray ray = _camera->generate_ray(sample);
                if (intersect(ray, t, &si)) {
                    Normal n = normalize(si.pi.gi.n);
                    r += (int)(n.x * 127 + 128);
                    g += (int)(n.y * 127 + 128);
                    b += (int)(n.z * 127 + 128);
                }
            }
            cout << (r / samples) << " ";
            cout << (g / samples) << " ";
            cout << (b / samples) << " ";
        }
        cout << endl;
    }
    auto end_time = clock();
    cerr << "resolution:[" << res_x << "," << res_y << "]" << endl;
    cerr << "samples_per_pixel:1" << endl;
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
