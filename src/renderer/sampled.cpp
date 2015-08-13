#include <ctime>
#include <cmath>
#include <thread>
#include <chrono>

#include "renderer/sampled.h"
#include "core/random.h"

namespace gill { namespace renderer {

using namespace std;
using namespace std::chrono;

inline Vector fuzz_normal(const Normal &n, const Sample &sample) {
    if (n.x > n.y && n.x > n.z) {
        return normalize(Vector(n.x, n.y + sample.lens_u - 0.5, n.z + sample.lens_v - 0.5));
    } else if (n.y > n.x && n.y > n.z) {
        return normalize(Vector(n.x + sample.lens_u - 0.5, n.y, n.z + sample.lens_v - 0.5));
    } else {
        return normalize(Vector(n.x + sample.lens_u - 0.5, n.y + sample.lens_v - 0.5, n.z));
    }
};

Spectrum trace(int level, const Sample &sample, const Ray &ray, const Scene *scene) {
    if (level < 0) {
        return Spectrum(0.f);
    }

    Intersection isec;
    float t = Infinity;
    if (scene->intersect(ray, t, &isec)) {
        if (!is_black(isec.emit)) {
            return isec.emit;
        } else if (!is_black(isec.refl)) {
            Vector next_normal = fuzz_normal(isec.n, sample);
            Ray next_ray(isec.p + next_normal * 0.01, next_normal);
            return isec.refl * trace(level - 1, sample, next_ray, scene);
        }
    }

    return Spectrum(0.f);
}

void render_tile(const Scene *scene, const Camera *camera, Sampler *sampler) {
    Sample *samples = new Sample[sampler->max_batch_size()];
    RNG rng;
    int count;
    while ((count = sampler->get_sample_batch(samples, rng)) > 0) {
        for (int i = 0; i < count; ++i) {
            Ray ray = camera->generate_ray(samples[i]);
            camera->_film->add_sample(samples[i], trace(5, samples[i], ray, scene));
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
