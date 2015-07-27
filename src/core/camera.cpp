#include "core/camera.h"

namespace gill { namespace core {

using namespace std;

Camera::Camera(shared_ptr<Transform> ltow, shared_ptr<Film> film)
    : _film(film), _ltow(ltow) {}

ProjectiveCamera::ProjectiveCamera(shared_ptr<Transform> ltow, shared_ptr<Transform> projection,
            float lens_radius, float focal_dist, shared_ptr<Film> film) : Camera(ltow, film) {
    _lens_radius = lens_radius;
    _focal_dist = focal_dist;
    _cam_to_screen = projection;
    vector<shared_ptr<Transform>> xforms;
    xforms.push_back(Transform::translate(0.5 * film->_xdim, 0.5 * film->_ydim, 0.0));
    xforms.push_back(Transform::scale(1.0 / film->_xdim, 1.0 / film->_ydim, 1.0));
    xforms.push_back(Transform::scale(float(film->_xres), float(film->_yres), 1.0));
    _screen_to_film = Transform::compose(xforms);
    _film_to_screen = make_shared<Transform>(inverse(*_screen_to_film));
    _film_to_cam = make_shared<Transform>(inverse(*_cam_to_screen) * (*_film_to_screen));
}

}}
