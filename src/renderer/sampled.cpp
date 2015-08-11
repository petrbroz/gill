#include <ctime>
#include <cmath>
#include <thread>
#include <chrono>

#include "renderer/sampled.h"
#include "core/random.h"

namespace gill { namespace renderer {

using namespace std;
using namespace std::chrono;

void render_tile(const Scene *scene, const Camera *camera, Sampler *sampler) {
    Camera::Sample *samples = new Camera::Sample[sampler->max_batch_size()];
    RNG rng;
    int count;
    while ((count = sampler->get_sample_batch(samples, rng)) > 0) {
        float r = 0.0, g = 0.0, b = 0.0;
        int sum = 0;
        for (int i = 0; i < count; ++i) {
            Ray ray = camera->generate_ray(samples[i]);
            Intersection isec;
            float t = Infinity;
            if (scene->intersect(ray, t, &isec)) {
                Normal n = normalize(isec.n);
                r += n.x * 0.5 + 0.5;
                g += n.y * 0.5 + 0.5;
                b += n.z * 0.5 + 0.5;
                sum++;
            }
        }
        if (sum > 0) {
            camera->_film->set_sample(floor(samples[0].image_x), floor(samples[0].image_y), Spectrum(r / count, g / count, b / count));
        }
    }
    delete[] samples;
}

SampledRenderer::SampledRenderer(shared_ptr<Camera> camera, shared_ptr<Sampler> sampler, int thread_tiles[2])
    : Renderer(camera), _sampler(sampler) {
    _thread_tiles[0] = thread_tiles[0];
    _thread_tiles[1] = thread_tiles[1];
}

void SampledRenderer::render(const Scene *scene) const {
    int res_x = _camera->_film->_xres;
    int res_y = _camera->_film->_yres;

    auto begin_time = high_resolution_clock::now();
    if (_thread_tiles[0] > 1 || _thread_tiles[1] > 1) {
        vector<Sampler *> subsamplers;
        vector<thread> threads;
        for (int j = 0; j < _thread_tiles[1]; ++j) {
            for (int i = 0; i < _thread_tiles[0]; ++i) {
                Sampler * subsampler = _sampler->get_subsampler(_thread_tiles[0], _thread_tiles[1], i, j);
                subsamplers.push_back(subsampler);
                threads.push_back(thread(render_tile, scene, _camera.get(), subsampler));
            }
        }
        for (auto &t : threads) {
            t.join();
        }
        for (auto &s : subsamplers) {
            delete s;
        }
    } else {
        render_tile(scene, _camera.get(), _sampler.get());
    }
    auto end_time = high_resolution_clock::now();
    duration<double, std::milli> elapsed = end_time - begin_time;

    _camera->_film->print_ppm();
    cerr << "resolution:[" << res_x << "," << res_y << "]" << endl;
    cerr << "thread_tiles:[" << _thread_tiles[0] << "," << _thread_tiles[1] << "]" << endl;
    cerr << "render_time:" << elapsed.count() << "ms" << endl;
}

}}
