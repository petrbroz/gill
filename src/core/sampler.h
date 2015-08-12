#ifndef GILL_CORE_SAMPLER_H_
#define GILL_CORE_SAMPLER_H_

#include "core/ray.h"
#include "core/random.h"
#include "core/spectrum.h"

namespace gill { namespace core {

/**
 * Ray generation input.
 */
struct Sample {
    float image_x, image_y;
    float lens_u, lens_v;
};

/**
 * Abstraction of an algorithm generating image samples to be processed by the renderer.
 */
class Sampler {
public:
    /**
     * Initializes the sampler for a given segment of the output image (in pixels).
     * @param x_min Minimum X-value of the image segment.
     * @param x_max Maximum X-value of the image segment.
     * @param y_min Minimum Y-value of the image segment.
     * @param y_max Maximum Y-value of the image segment.
     */
    Sampler(int x_min, int x_max, int y_min, int y_max);
    virtual ~Sampler();

    /**
     * Provides maximum number of samples the sampler generates in one batch.
     */
    virtual int max_batch_size() const = 0;

    /**
     * Generates a number of samples (up to the maximum defined by Sampler::max_batch_size).
     * @param samples Array of samples to be populated.
     * @param rng Random number generator.
     * @returns Number of generated samples. If 0, the sampler has finished its work.
     */
    virtual int get_sample_batch(Sample *samples, RNG &rng) = 0;

    /**
     * Accepts results for the generated samples, potentially adjusting sampler's next choices.
     * @param samples Samples for which the outputs are reported.
     * @param rays Rays computed for each sample.
     * @param radiances L-values computed for each sample.
     * @param count Number of samples, rays and radiances being reported.
     * @returns True if the reported results should be included in the output.
     */
    virtual bool report_results(Sample *samples, const Ray *rays, const Spectrum *radiances, int count);

    /**
     * Creates a subsampler for multithreaded/multitiled workflows.
     * @param h_tiles Total number of column tiles.
     * @param v_tiles Total number of row tiles.
     * @param i Current tile's column.
     * @param j Current tile's row.
     * @returns New sampler instance for processing a subset of this sampler's input.
     */
    virtual Sampler * get_subsampler(int h_tiles, int v_tiles, int i, int j) = 0;

protected:
    void compute_subwindow(int h_tiles, int v_tiles, int i, int j, int *x_min, int *x_max, int *y_min, int *y_max) const;
    int _x_min, _x_max, _y_min, _y_max;
};

}}

#endif
