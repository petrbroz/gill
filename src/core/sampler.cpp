#include "core/sampler.h"
#include "core/random.h"

namespace gill { namespace core {

using namespace std;

Sampler::Sampler(int x_min, int x_max, int y_min, int y_max, int spp)
    : _x_min(x_min), _x_max(x_max), _y_min(y_min), _y_max(y_max), _spp(spp) {}

Sampler::~Sampler() {}

}}
