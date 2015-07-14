#ifndef GILL_PARSER_H_
#define GILL_PARSER_H_

#include <yaml.h>
#include <vector>

#include "camera.h"
#include "film.h"
#include "material.h"
#include "mesh.h"
#include "primitive.h"
#include "scene.h"

namespace gill {

class Parser {
public:
    Parser();
    ~Parser();
    std::shared_ptr<core::Scene> next_scene();

protected:
    yaml_parser_t _parser;

    std::shared_ptr<core::Scene> parse_scene(yaml_document_t *doc, yaml_node_t *node);
    std::vector<core::Primitive> parse_primitives(yaml_document_t *doc, yaml_node_t *node);
    core::Primitive parse_primitive(yaml_document_t *doc, yaml_node_t *node);
    std::shared_ptr<core::Mesh> parse_geometry(yaml_document_t *doc, yaml_node_t *node);
    std::shared_ptr<core::Material> parse_material(yaml_document_t *doc, yaml_node_t *node);
    std::shared_ptr<core::Transform> parse_transform(yaml_document_t *doc, yaml_node_t *node);
    core::Camera parse_camera(yaml_document_t *doc, yaml_node_t *node);
    core::Film parse_film(yaml_document_t *doc, yaml_node_t *node);
    core::Scene::Settings parse_settings(yaml_document_t *doc, yaml_node_t *node);
};

}

#endif
