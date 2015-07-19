#ifndef GILL_CORE_PARSER_H_
#define GILL_CORE_PARSER_H_

#include <yaml.h>
#include <vector>

#include "core/camera.h"
#include "core/film.h"
#include "core/material.h"
#include "core/primitive.h"
#include "core/scene.h"
#include "geometry/mesh.h"

using namespace gill::geometry;

namespace gill { namespace core {

class Parser {
public:
    Parser();
    Parser(const char *filename);
    ~Parser();
    std::shared_ptr<Scene> next_scene();

protected:
    yaml_parser_t _parser;
    FILE *_input;

    std::shared_ptr<Scene> parse_scene(yaml_document_t *doc, yaml_node_t *node);
    std::vector<Primitive> parse_primitives(yaml_document_t *doc, yaml_node_t *node);
    Primitive parse_primitive(yaml_document_t *doc, yaml_node_t *node);
    std::shared_ptr<Mesh> parse_geometry(yaml_document_t *doc, yaml_node_t *node);
    std::shared_ptr<Material> parse_material(yaml_document_t *doc, yaml_node_t *node);
    std::shared_ptr<Transform> parse_transform(yaml_document_t *doc, yaml_node_t *node);
    Camera parse_camera(yaml_document_t *doc, yaml_node_t *node);
    Film parse_film(yaml_document_t *doc, yaml_node_t *node);
    //Scene::Settings parse_settings(yaml_document_t *doc, yaml_node_t *node);
};

}}

#endif
