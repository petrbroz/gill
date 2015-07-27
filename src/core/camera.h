#ifndef GILL_CORE_CAMERA_H_
#define GILL_CORE_CAMERA_H_

#include <iostream>

#include "core/transform.h"
#include "core/film.h"

namespace gill { namespace core {

class Camera {
public:
    struct Sample {
        float image_x, image_y;
        float lens_u, lens_v;
    };

    Camera(std::shared_ptr<Transform> ltow, std::shared_ptr<Film> film);
    virtual Ray generate_ray(const Sample &sample) const = 0;

    std::shared_ptr<Film> _film;
protected:
    std::shared_ptr<Transform> _ltow;
};

class ProjectiveCamera : public Camera {
public:
    ProjectiveCamera(std::shared_ptr<Transform> ltow, std::shared_ptr<Transform> projection,
            float lens_radius, float focal_dist, std::shared_ptr<Film> film);

protected:
    float _lens_radius;
    float _focal_dist;
    std::shared_ptr<Transform> _cam_to_screen;
    std::shared_ptr<Transform> _screen_to_film;
    std::shared_ptr<Transform> _film_to_screen;
    std::shared_ptr<Transform> _film_to_cam;
};

}}

#endif
