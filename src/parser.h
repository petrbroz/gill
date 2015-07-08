#ifndef GILL_PARSER_H_
#define GILL_PARSER_H_

#include "primitive.h"
#include "mesh.h"
#include "transform.h"

typedef struct _scene {
    std::vector<gill::core::Primitive> primitives;
    std::vector<gill::core::Mesh> meshes;
    std::vector<gill::core::Transform> transforms;
    struct {
        float position[3];
        float direction[3];
    } camera;
    struct {
        int frame[4];
    } settings;
} scene_t;

void parse_input(FILE *input, scene_t *scene);

#endif
