#ifndef GILL_CORE_PARSER_H_
#define GILL_CORE_PARSER_H_

#include <yaml.h>
#include <vector>
#include <map>
#include <functional>

#include "core/camera.h"
#include "core/film.h"
#include "core/geometry.h"
#include "core/material.h"
#include "core/primitive.h"
#include "core/renderer.h"
#include "core/sampler.h"
#include "core/scene.h"

namespace gill { namespace core {

/**
 * Parser of YAML-based scene description documents.
 */
class Parser {
public:
    struct Document {
        std::shared_ptr<Scene> scene;
        std::shared_ptr<Renderer> renderer;
    };

    Parser();
    Parser(const char *filename);
    ~Parser();
    std::shared_ptr<Document> next_document();

protected:
    FILE *_input;
    yaml_parser_t _parser;
    yaml_document_t _document;

    std::map<int, std::shared_ptr<Transform>> _transforms;
    std::map<int, std::shared_ptr<Geometry>> _geometries;
    std::map<int, std::shared_ptr<Material>> _materials;

    std::shared_ptr<Document> parse_document(yaml_node_t *node);
    std::shared_ptr<Scene> parse_scene(yaml_node_t *node);
    std::vector<Primitive> parse_primitives(yaml_node_t *node);
    Primitive parse_primitive(yaml_node_t *node);
    std::shared_ptr<Geometry> parse_geometry(yaml_node_t *node);
    std::shared_ptr<Material> parse_material(yaml_node_t *node);
    std::shared_ptr<Transform> parse_transform(yaml_node_t *node);
    std::shared_ptr<Renderer> parse_renderer(yaml_node_t *node);
    std::shared_ptr<Sampler> parse_sampler(yaml_node_t *node);
    std::shared_ptr<Camera> parse_camera(yaml_node_t *node);
    std::shared_ptr<Film> parse_film(yaml_node_t *node);
    std::shared_ptr<Filter> parse_filter(yaml_node_t *node);

    void _traverse_mapping(yaml_node_t *node, std::function<void(std::string&, yaml_node_t*)> func);
    void _traverse_sequence(yaml_node_t *node, std::function<void(yaml_node_t*)> func);
    template <typename T> T _get_scalar(yaml_node_t *node);
    template <typename T, int size> std::vector<T> _get_sequence(yaml_node_t *node);
};

}}

#endif
