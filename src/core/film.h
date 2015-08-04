#ifndef GILL_CORE_FILM_H_
#define GILL_CORE_FILM_H_

#include <iostream>
#include "core/spectrum.h"

namespace gill { namespace core {

/**
 * Medium for capturing the rendering results.
 */
class Film {
public:
    int _xres, _yres; /// Resolution of the film
    float _xdim, _ydim; /// Physical dimensions of the film
    Spectrum *data;

    Film(int xres, int yres) : _xres(xres), _yres(yres) {
        _xdim = 2.0;
        _ydim = _xdim * (_yres / _xres);
        data = new Spectrum[_xres * _yres];
    }

    void set_sample(int x, int y, const Spectrum &spectrum) {
        data[y * _xres + x] = spectrum;
    }

    Spectrum get_sample(int x, int y) const {
        return data[y * _xres + x];
    }

    ~Film() {
        delete[] data;
    }

    void print_ppm() const {
        std::cout << "P3" << std::endl;
        std::cout << _xres << " " << _yres << std::endl;
        std::cout << "255" << std::endl;
        for (int y = 0; y < _yres; ++y) {
            for (int x = 0; x < _xres; ++x) {
                Spectrum spectrum = get_sample(x, _yres - 1 - y);
                std::cout << (int)(spectrum[0] * 255) << " ";
                std::cout << (int)(spectrum[1] * 255) << " ";
                std::cout << (int)(spectrum[2] * 255) << " ";
            }
            std::cout << std::endl;
        }
    }

    friend std::ostream& operator<<(std::ostream& out, const Film& film);
};

inline std::ostream& operator<<(std::ostream &out, const Film &film) {
    out << "{";
    out << "\"resolution\":[" << film._xres << "," << film._yres << "]";
    out << "}";
    return out;
}

}}

#endif
