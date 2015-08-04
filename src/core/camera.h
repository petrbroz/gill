#ifndef GILL_CORE_CAMERA_H_
#define GILL_CORE_CAMERA_H_

#include <iostream>

#include "core/transform.h"
#include "core/film.h"

namespace gill { namespace core {

/**
 * Abstraction of a camera model used to generate rays.
 */
class Camera {
public:

    /**
     * Ray generation input.
     */
    struct Sample {
        float image_x, image_y;
        float lens_u, lens_v;
    };

    /**
     * Initializes a camera.
     * @param ltow Local-to-world transformation. In its local space, the camera points in the +Z direction,
     * 'up' vector points in the +Y direction and 'right' vector points in the +X direction.
     * @param film Raster for capturing the output.
     */
    Camera(std::shared_ptr<Transform> ltow, std::shared_ptr<Film> film);

    /**
     * Generates a ray for given input sample.
     * @param sample Input sample.
     * @returns Ray, in world coordinate system, shot from the camera.
     */
    virtual Ray generate_ray(const Sample &sample) const = 0;

    std::shared_ptr<Film> _film;
protected:
    std::shared_ptr<Transform> _ltow;
};

/**
 * Specialization of gill::core::Camera using a projective transformation
 * for the camera-to-screen coordinate system relation.
 */
class ProjectiveCamera : public Camera {
public:

    /**
     * Initializes a projective camera.
     * @param ltow Local-to-world transformation.
     * @param projection Transformation from camera space to screen space.
     * @param lens_radius If larger than zero, defines size of the out-of-focus blur.
     * @param focal_dist Distance of the focal plane from the camera origin.
     * @param film Raster for capturing the output.
     */
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
