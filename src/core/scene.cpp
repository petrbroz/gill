#include <ctime>
#include <cmath>

#include "core/scene.h"

namespace gill { namespace core {

using namespace std;

Scene::Scene(std::vector<Primitive> primitives, Camera camera)
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
    Vector dir = _camera._look_at - _camera._position;
    int res_x = _camera._film._xres;
    int res_y = _camera._film._yres;
    float dim_x = 2.0 * length(dir) * tan(0.5 * radians(_camera._fov));
    float dim_y = dim_x * res_y / res_x;
    float dx = dim_x / res_x;
    float dy = dim_y / res_y;
    float x = -0.5 * dim_x;
    float y = +0.5 * dim_y;
    dir = normalize(dir);
    Vector up(0.0, 1.0, 0.0);
    Vector right = normalize(cross(up, dir));
    up = normalize(cross(dir, right));

    cout << "P3" << endl;
    cout << res_x << " " << res_y << endl;
    cout << "255" << endl;
    for (int i = 0; i < res_y; i += 1) {
        y -= dy;
        x = -0.5 * dim_x;
        for (int j = 0; j < res_x; j += 1) {
            x += dx;
            Ray ray(_camera._position, normalize(_camera._look_at + right * x + up * y - _camera._position));
            if (intersect(ray, t, &si)) {
                cout << (int)(si.pi.gi.n.x * 127 + 128) << " ";
                cout << (int)(si.pi.gi.n.y * 127 + 128) << " ";
                cout << (int)(si.pi.gi.n.z * 127 + 128) << " ";
            } else {
                cout << "0 0 0 ";
            }
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
