#ifndef GILL_CORE_PARSER_H_
#define GILL_CORE_PARSER_H_

#include <yaml.h>
#include <vector>

#include "core/aggregate.h"
#include "core/camera.h"
#include "core/film.h"
#include "core/material.h"
#include "core/primitive.h"
#include "core/scene.h"
#include "core/renderer.h"
#include "geometry/geometry.h"

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
    std::shared_ptr<Aggregate> parse_primitives(yaml_document_t *doc, yaml_node_t *node);
    Primitive parse_primitive(yaml_document_t *doc, yaml_node_t *node);
    std::shared_ptr<Geometry> parse_geometry(yaml_document_t *doc, yaml_node_t *node);
    std::shared_ptr<Material> parse_material(yaml_document_t *doc, yaml_node_t *node);
    std::shared_ptr<Transform> parse_transform(yaml_document_t *doc, yaml_node_t *node);
    std::shared_ptr<Camera> parse_camera(yaml_document_t *doc, yaml_node_t *node);
    std::shared_ptr<Film> parse_film(yaml_document_t *doc, yaml_node_t *node);
    std::shared_ptr<Renderer> parse_renderer(yaml_document_t *doc, yaml_node_t *node);
};

}}

#endif
