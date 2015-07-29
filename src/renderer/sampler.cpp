#include <ctime>
#include <cmath>
#include <thread>
#include <chrono>

#include "renderer/sampler.h"

namespace gill { namespace renderer {

using namespace std;
using namespace std::chrono;

void render_block(const Aggregate *aggregate, const Camera *camera, int minx, int miny, int maxx, int maxy, int samples) {
    for (int y = miny; y <= maxy; ++y) {
        for (int x = minx; x <= maxx; ++x) {
            float r = 0.0, g = 0.0, b = 0.0;
            for (int s = 0; s < samples; ++s) {
                Primitive::Intersection pi;
                float t = Infinity;
                float u = 2.0 * rand() / RAND_MAX - 1.0;
                float v = 2.0 * rand() / RAND_MAX - 1.0;
                Camera::Sample sample = {(float)x, (float)y, u, v};
                Ray ray = camera->generate_ray(sample);
                if (aggregate->intersect(ray, t, &pi)) {
                    Normal n = normalize(pi.gi.n);
                    r += n.x * 0.5 + 0.5;
                    g += n.y * 0.5 + 0.5;
                    b += n.z * 0.5 + 0.5;
                }
            }
            camera->_film->set_sample(x, y, Spectrum(r / samples, g / samples, b / samples));
        }
    }
}

Sampler::Sampler(int samples_per_pixel, int thread_tiles[2]) : _samples_per_pixel(samples_per_pixel) {
    _thread_tiles[0] = thread_tiles[0];
    _thread_tiles[1] = thread_tiles[1];
}

void Sampler::render(const Aggregate *aggregate, const Camera *camera) const {
    int res_x = camera->_film->_xres;
    int res_y = camera->_film->_yres;
    int seg_x = res_x / _thread_tiles[0];
    int seg_y = res_y / _thread_tiles[1];

    auto begin_time = high_resolution_clock::now();
    if (_thread_tiles[0] > 1 || _thread_tiles[1] > 1) {
        vector<thread> threads;
        for (int j = 0; j < _thread_tiles[1]; ++j) {
            int min_y = j * seg_y;
            int max_y = (j + 1) * seg_y - 1;
            for (int i = 0; i < _thread_tiles[0]; ++i) {
                int min_x = i * seg_x;
                int max_x = (i + 1) * seg_x - 1;
                threads.push_back(thread(render_block, aggregate, camera, min_x, min_y, max_x, max_y, _samples_per_pixel));
            }
        }
        for (thread &t : threads) {
            t.join();
        }
    } else {
        render_block(aggregate, camera, 0, 0, res_x - 1, res_y - 1, _samples_per_pixel);
    }
    auto end_time = high_resolution_clock::now();
    duration<double, std::milli> elapsed = end_time - begin_time;

    camera->_film->print_ppm();
    cerr << "resolution:[" << res_x << "," << res_y << "]" << endl;
    cerr << "samples_per_pixel:" << _samples_per_pixel << endl;
    cerr << "thread_tiles:[" << _thread_tiles[0] << "," << _thread_tiles[1] << "]" << endl;
    cerr << "render_time:" << elapsed.count() << "ms" << endl;
}

}}