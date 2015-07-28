#include <vector>
#include <algorithm>
#include "core/math.h"

namespace gill { namespace core {

using namespace std;

struct Sample {
    float x, y;
    bool operator<(const Sample &rhs) const { return x < rhs.x; }
};

float average_samples(const vector<Sample> &samples, float x0, float x1) {
    int num_samples = samples.size();
    if (num_samples == 1) {
        return samples[0].y;
    } else if (x1 <= samples[0].x) {
        return samples[0].y;
    } else if (x0 >= samples[num_samples - 1].x) {
        return samples[num_samples - 1].y;
    }

    float sum = 0.0;
    if (x0 < samples[0].x) {
        sum += samples[0].y * (samples[0].x - x0);
    }
    if (x1 > samples[num_samples - 1].x) {
        sum += samples[num_samples - 1].y * (x1 - samples[num_samples - 1].x);
    }

    for (int i = 0; i < num_samples - 1; ++i) {
        const Sample &s0 = samples[i];
        const Sample &s1 = samples[i + 1];
        if (s0.x >= x0 && s1.x <= x1) {
            sum += 0.5 * (s0.y + s1.y) * (s1.x - s0.x);
        } else if (s0.x < x0 && s1.x > x0) {
            float tmp = lerp(s0.y, s1.y, (x0 - s0.x) / (s1.x - s0.x));
            sum += 0.5 * (tmp + s1.y) * (s1.x - x0);
        } else if (s0.x < x1 && s1.x > x1) {
            float tmp = lerp(s0.y, s1.y, (x1 - s0.x) / (s1.x - s0.x));
            sum += 0.5 * (s0.y + tmp) * (x1 - s0.x);
        }
    }

    return sum / (x1 - x0);
}

void resample(const float *xi, const float *yi, int ni, float *yo, float min_xo, float max_xo, int no) {
    vector<Sample> samples;
    samples.reserve(ni);
    for (int i = 0; i < ni; ++i) {
        samples.push_back({xi[i], yi[i]});
    }
    sort(samples.begin(), samples.end());
    for (int i = 0; i < no; ++i) {
        float x0 = lerp(min_xo, max_xo, float(i) / no);
        float x1 = lerp(min_xo, max_xo, float(i + 1) / no);
        yo[i] = average_samples(samples, x0, x1);
    }
}

}}
