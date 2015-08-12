#ifndef GILL_CAMERA_PERSPECTIVE_H_
#define GILL_CAMERA_PERSPECTIVE_H_

#include <iostream>

#include "core/camera.h"
#include "core/transform.h"
#include "core/film.h"
#include "core/ray.h"

namespace gill { namespace camera {

using namespace gill::core;

/**
 * Perspective projection camera.
 */
class PerspectiveCamera : public ProjectiveCamera {
public:
    PerspectiveCamera(std::shared_ptr<Transform> ltow, std::shared_ptr<Film> film,
            float fov, float lens_radius, float focal_dist);
    Ray generate_ray(const Sample &sample) const;
};

}}

#endif
