#ifndef GILL_CORE_FILM_H_
#define GILL_CORE_FILM_H_

#include <iostream>

namespace gill { namespace core {

class Film {
public:
    int _xres, _yres;

    Film(int xres, int yres) : _xres(xres), _yres(yres) {}
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
