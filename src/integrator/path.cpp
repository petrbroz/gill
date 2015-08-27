#include "integrator/path.h"
#include "core/montecarlo.h"

namespace gill { namespace integrator {

Spectrum trace(int level, const Ray &ray, const Scene *scene, const Sample &sample) {
    if (level < 0) {
        return Spectrum(0.f);
    }

    Intersection isec;
    float t = Infinity;
    if (scene->intersect(ray, t, &isec)) {
        if (!is_black(isec.emit)) {
            return isec.emit;
        } else if (!is_black(isec.refl)) {
            Vector next_normal = uniform_hemisphere_sample(sample.lens_u, sample.lens_v);
            next_normal = normalize(isec.n + next_normal);
            /*
            if (isec.dpdu.x != 0.f || isec.dpdu.y != 0.f || isec.dpdu.z != 0.f) {
                isec.n = normalize(isec.n);
                Vector tmp1 = normalize(isec.dpdu);
                Vector tmp2 = normalize(cross(tmp1, isec.n));
                auto xform = Transform::coord_sys(tmp1, tmp2, isec.n);
                next_normal = normalize((*xform)(next_normal));
            }
            */
            Ray next_ray(isec.p + next_normal * 0.01, next_normal);
            return isec.refl * trace(level - 1, next_ray, scene, sample);
        }
    }

    return Spectrum(0.f);
}

Spectrum PathIntegrator::Li(const Ray &ray, const Scene *scene, const Sample &sample) const {
    return trace(_max_depth, ray, scene, sample);
}

}}
