#ifndef GILL_CORE_FILM_H_
#define GILL_CORE_FILM_H_

#include <cmath>
#include <algorithm>
#include "core/filter.h"
#include "core/spectrum.h"
#include "core/sampler.h"

namespace gill { namespace core {

using namespace std;

/**
 * Medium for capturing the rendering results.
 */
class Film {
public:
    struct Pixel {
        Spectrum radiance;
        float weight;
    };

    const int FilterTableSize = 16;

    int _xres, _yres; /// Resolution of the film
    float _xdim, _ydim; /// Physical dimensions of the film
    shared_ptr<Filter> _filter;
    float *_filter_table;
    Pixel *data;

    Film(int xres, int yres, shared_ptr<Filter> filter) : _xres(xres), _yres(yres), _filter(filter) {
        _xdim = 2.0;
        _ydim = _xdim * (_yres / _xres);
        data = new Pixel[_xres * _yres];

        _filter_table = new float[FilterTableSize * FilterTableSize];
        int i = 0;
        for (int y = 0; y < FilterTableSize; ++y) {
            float _y = (0.5f + y) * _filter->height() / FilterTableSize;
            for (int x = 0; x < FilterTableSize; ++x) {
                float _x = (0.5f + x) * _filter->width() / FilterTableSize;
                _filter_table[i] = _filter->evaluate(_x, _y);
                i++;
            }
        }
    }

    void add_sample(const Sample &sample, const Spectrum &radiance) {
        float dx = sample.image_x - 0.5f;
        float dy = sample.image_y - 0.5f;
        int x0 = std::max(0, (int)(ceil(dx - _filter->width())));
        int x1 = std::min(_xres - 1, (int)(floor(dx + _filter->width())));
        int y0 = std::max(0, (int)(ceil(dy - _filter->height())));
        int y1 = std::min(_yres - 1, (int)floor(dy + _filter->height()));
        if ((x1 - x0) < 0 || (y1 - y0) < 0) {
            cerr << "sample outside of image extent" << endl;
            return;
        }
        for (int y = y0; y <= y1; ++y) {
            for (int x = x0; x <= x1; ++x) {
                int fx = FilterTableSize - 1, fy = FilterTableSize - 1;
                fx = std::min(fx, (int)(floor(abs(x - dx) * _filter->inv_width() * FilterTableSize)));
                fy = std::min(fy, (int)(floor(abs(y - dy) * _filter->inv_height() * FilterTableSize)));
                float weight = _filter_table[fy * FilterTableSize + fx];
                Pixel &pixel = data[y * _xres + x];
                pixel.radiance += radiance * weight;
                pixel.weight += weight;
            }
        }
    }

    Spectrum get_pixel(int x, int y) const {
        const Pixel &pixel = data[y * _xres + x];
        //cerr << "got radiance " << pixel.radiance[0] << " " << pixel.radiance[1] << " " << pixel.radiance[2] << " with weight " << pixel.weight << endl;
        if (almost_zero(pixel.weight)) {
            return Spectrum(0.0);
        } else {
            return clamp(pixel.radiance / pixel.weight, 0.f, 1.f);
        }
    }

    ~Film() {
        delete[] _filter_table;
        delete[] data;
    }

    void print_ppm() const {
        std::cout << "P3" << std::endl;
        std::cout << _xres << " " << _yres << std::endl;
        std::cout << "255" << std::endl;
        for (int y = 0; y < _yres; ++y) {
            for (int x = 0; x < _xres; ++x) {
                Spectrum spectrum = get_pixel(x, _yres - 1 - y);
                std::cout << (int)(spectrum[0] * 255) << " ";
                std::cout << (int)(spectrum[1] * 255) << " ";
                std::cout << (int)(spectrum[2] * 255) << " ";
            }
            std::cout << std::endl;
        }
    }

    friend std::ostream& operator<<(std::ostream& out, const Film& film);
};

}}

#endif
