#include <ctime>
#include <cmath>
#include <thread>
#include <chrono>

#include "renderer/sampled.h"
#include "core/random.h"
#include "core/montecarlo.h"

namespace gill { namespace renderer {

using namespace std;
using namespace std::chrono;

void render_tile(const SurfaceIntegrator *si, const Scene *scene, const Camera *camera, Sampler *sampler) {
    Sample *samples = new Sample[sampler->max_batch_size()];
    RNG rng;
    int count;
    while ((count = sampler->get_sample_batch(samples, rng)) > 0) {
        for (int i = 0; i < count; ++i) {
            Ray ray = camera->generate_ray(samples[i]);
            camera->_film->add_sample(samples[i], si->Li(ray, scene, samples[i]));
        }
    }
    delete[] samples;
}

SampledRenderer::SampledRenderer(shared_ptr<Camera> camera, shared_ptr<SurfaceIntegrator> surface_integrator,
        shared_ptr<Sampler> sampler, int thread_tiles[2])
    : Renderer(camera, surface_integrator), _sampler(sampler) {
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
                threads.push_back(thread(render_tile, _surface_integrator.get(), scene, _camera.get(), subsampler));
            }
        }
        for (auto &t : threads) {
            t.join();
        }
        for (auto &s : subsamplers) {
            delete s;
        }
    } else {
        render_tile(_surface_integrator.get(), scene, _camera.get(), _sampler.get());
    }
    auto end_time = high_resolution_clock::now();
    duration<double, std::milli> elapsed = end_time - begin_time;

    _camera->_film->print_ppm();
    cerr << "resolution:[" << res_x << "," << res_y << "]" << endl;
    cerr << "thread_tiles:[" << _thread_tiles[0] << "," << _thread_tiles[1] << "]" << endl;
    cerr << "render_time:" << elapsed.count() << "ms" << endl;
}

}}
