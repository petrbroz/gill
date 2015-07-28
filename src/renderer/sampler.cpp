#include <ctime>
#include <cmath>
#include <thread>

#include "renderer/sampler.h"

namespace gill { namespace renderer {

using namespace std;

const int samples = 32;

void render_fragment(const Aggregate *aggregate, const Camera *camera, int minx, int miny, int maxx, int maxy) {
    float t;
    Primitive::Intersection pi;
    for (int y = miny; y <= maxy; ++y) {
        for (int x = minx; x <= maxx; ++x) {
            float r = 0.0, g = 0.0, b = 0.0;
            for (int s = 0; s < samples; ++s) {
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

    auto begin_time = clock();
    vector<thread> threads;
    for (int j = 0; j < _thread_tiles[0]; ++j) {
        int min_y = j * seg_y;
        int max_y = (j + 1) * seg_y - 1;
        for (int i = 0; i < _thread_tiles[1]; ++i) {
            int min_x = i * seg_x;
            int max_x = (i + 1) * seg_x - 1;
            threads.push_back(thread(render_fragment, aggregate, camera, min_x, min_y, max_x, max_y));
        }
    }
    for (thread &t : threads) {
        t.join();
    }
    auto end_time = clock();

    camera->_film->print_ppm();
    cerr << "resolution:[" << res_x << "," << res_y << "]" << endl;
    cerr << "samples_per_pixel:" << _samples_per_pixel << endl;
    cerr << "thread_tiles:[" << _thread_tiles[0] << "," << _thread_tiles[1] << "]" << endl;
    cerr << "render_time:" << float(end_time - begin_time) / CLOCKS_PER_SEC << "s" << endl;
}

}}
