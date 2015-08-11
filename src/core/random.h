#ifndef GILL_CORE_RANDOMIZER_H_
#define GILL_CORE_RANDOMIZER_H_

#include <random>

namespace gill { namespace core {

template<typename T>
T random(std::mt19937 &rng, const T &min, const T &max) {
    std::uniform_int_distribution<T> distribution(min, max);
    return distribution(rng);
}

}}

#endif
