#include "camera/perspective.h"
#include "core/vector.h"

namespace gill { namespace camera {

using namespace std;

PerspectiveCamera::PerspectiveCamera(shared_ptr<Transform> ltow, shared_ptr<Film> film,
        float fov, float lens_radius, float focal_dist)
    : ProjectiveCamera(ltow, Transform::perspective(fov, 1.0e-2, 1.0e3), lens_radius, focal_dist, film) {}

Ray PerspectiveCamera::generate_ray(const Camera::Sample &sample) const {
    Point img_p(sample.image_x, sample.image_y, 0.0);
    Point cam_p = (*_film_to_cam)(img_p);
    Ray ray(Point(0.0), normalize(Vector(cam_p.x, cam_p.y, cam_p.z)));

    if (_lens_radius > 0.0) {
        float focus_t = _focal_dist / ray.d.z;
        Point focus_p = ray(focus_t);
        ray.o = Point(sample.lens_u * _lens_radius, sample.lens_v * _lens_radius, 0.0); // TODO: map lens_u and lens_v properly
        ray.d = normalize(focus_p - ray.o);
    }
    return (*_ltow)(ray);
}

}}
