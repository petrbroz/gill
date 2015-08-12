#ifndef GILL_CORE_RANDOMIZER_H_
#define GILL_CORE_RANDOMIZER_H_

#include <random>

namespace gill { namespace core {

typedef std::mt19937 RNG;

inline float random_float(RNG &rng, const float &min, const float &max) {
    std::uniform_real_distribution<float> distribution(min, max);
    return distribution(rng);
}

inline int random_int(RNG &rng, const int &min, const int &max) {
    std::uniform_int_distribution<int> distribution(min, max);
    return distribution(rng);
}

}}

#endif
