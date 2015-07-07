#ifndef GILL_CORE_PARSER_H_
#define GILL_CORE_PARSER_H_

#include "primitive.h"
#include "mesh.h"

namespace gill { namespace core {

typedef struct _scene {
    std::vector<Primitive> primitives;
    std::vector<Mesh> meshes;
    std::vector<Transform> transforms;
    struct {
        float position[3];
        float direction[3];
    } camera;
    struct {
        int frame[4];
    } settings;
} scene_t;

void parse_input(FILE *input, scene_t *scene);

}}

#endif
