#ifndef GILL_CORE_CAMERA_H_
#define GILL_CORE_CAMERA_H_

#include <iostream>
#include "vector.h"
#include "film.h"

namespace gill { namespace core {

class Camera {
public:
    Point _position;
    Point _look_at;
    float _fov;
    Film _film;

    Camera(const Point &position, const Point &look_at, float fov, const Film &film)
        : _position(position), _look_at(look_at), _fov(fov), _film(film) {}
    friend std::ostream& operator<<(std::ostream& out, const Camera& camera);
};

inline std::ostream& operator<<(std::ostream &out, const Camera &camera) {
    out << "{";
    out << "\"position\":" << camera._position << ",";
    out << "\"look_at\":" << camera._look_at << ",";
    out << "\"fov\":" << camera._fov << ",";
    out << "\"film\":" << camera._film;
    out << "}";
    return out;
}

}}

#endif
