#include "scene.h"

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
    int xres = _camera._film._xres / 2;
    int yres = _camera._film._yres / 2;
    float t;
    Scene::Intersection si;
    Vector dir = normalize(_camera._look_at - _camera._position);
    Vector up(0.0, 1.0, 0.0);
    Vector right = normalize(cross(up, dir));
    up = normalize(cross(dir, right));

    cout << "P3" << endl;
    cout << (_camera._film._xres) << " " << (_camera._film._yres) << endl;
    cout << "255" << endl;
    for (int y = +yres; y > -yres; --y) {
        for (int x = -xres; x < +xres; ++x) {
            Ray ray(_camera._position, normalize((_camera._look_at + right * (x * 1.1) + up * (y * 1.1)) - _camera._position));
            if (intersect(ray, t, &si)) {
                cout << (int)(si.pi.mi.ti.n.x * 128 + 127) << " ";
                cout << (int)(si.pi.mi.ti.n.y * 128 + 127) << " ";
                cout << (int)(si.pi.mi.ti.n.z * 128 + 127) << " ";
            } else {
                cout << "0 0 0 ";
            }
        }
        cout << endl;
    }
}

bool Scene::intersect(const Ray &ray, float &t, Scene::Intersection *si) const {
    Primitive::Intersection *pi = nullptr;
    if (si) {
        pi = &(si->pi);
    }
    return _accelerator->intersect(_primitives, ray, t, pi);
}

}}
