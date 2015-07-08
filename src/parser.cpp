#include <cassert>
#include <yaml.h>
#include "parser.h"

using namespace std;
using namespace gill::core;

void parse_document(yaml_node_t *node, yaml_document_t *doc, scene_t *scene);
void parse_primitives(yaml_node_t *node, yaml_document_t *doc, scene_t *scene);
void parse_primitive(yaml_node_t *node, yaml_document_t *doc, scene_t *scene);
void parse_geometry(yaml_node_t *node, yaml_document_t *doc, Primitive *primitive);
void parse_material(yaml_node_t *node, yaml_document_t *doc, Primitive *primitive);
void parse_transforms(yaml_node_t *node, yaml_document_t *doc, Primitive *primitive);
void parse_camera(yaml_node_t *node, yaml_document_t *doc, scene_t *scene);
void parse_settings(yaml_node_t *node, yaml_document_t *doc, scene_t *scene);
template <typename T>T get_scalar(yaml_node_t *node);
template <typename T, int size> void get_sequence(yaml_node_t *node, yaml_document_t *doc, T *values);

void parse_input(FILE *input, scene_t *scene) {
    yaml_parser_t parser;
    yaml_document_t doc;
    yaml_parser_initialize(&parser);
    yaml_parser_set_input_file(&parser, input);
    yaml_parser_load(&parser, &doc);
    auto *root = yaml_document_get_root_node(&doc);
    parse_document(root, &doc, scene);
    yaml_document_delete(&doc);
    yaml_parser_delete(&parser);
}

void parse_document(yaml_node_t *node, yaml_document_t *doc, scene_t *scene) {
    assert(node->type == YAML_MAPPING_NODE);
    auto mapping = node->data.mapping;
    for (auto *item = mapping.pairs.start; item != mapping.pairs.top; item++) {
        auto knode = yaml_document_get_node(doc, item->key);
        auto vnode = yaml_document_get_node(doc, item->value);
        string key = get_scalar<string>(knode);
        if (key == "primitives") {
            parse_primitives(vnode, doc, scene);
        } else if (key == "camera") {
            parse_camera(vnode, doc, scene);
        } else if (key == "settings") {
            parse_settings(vnode, doc, scene);
        }
    }
}

void parse_primitives(yaml_node_t *node, yaml_document_t *doc, scene_t *scene) {
    assert(node->type == YAML_SEQUENCE_NODE);
    auto seq = node->data.sequence;
    for (auto *item = seq.items.start; item != seq.items.top; item++) {
        auto inode = yaml_document_get_node(doc, *item);
        parse_primitive(inode, doc, scene);
    }
}

void parse_primitive(yaml_node_t *node, yaml_document_t *doc, scene_t *scene) {
    assert(node->type == YAML_MAPPING_NODE);
    //Mesh *mesh = nullptr;
    //Material *material;
    //Transform *transform = nullptr;
    auto mapping = node->data.mapping;
    for (auto *item = mapping.pairs.start; item != mapping.pairs.top; item++) {
        auto knode = yaml_document_get_node(doc, item->key);
        //auto vnode = yaml_document_get_node(doc, item->value);
        string key = get_scalar<string>(knode);
        if (key == "geometry") {
            //parse_geometry(vnode, doc, primitive);
        } else if (key == "material") {
            //parse_material(vnode, doc, primitive);
        } else if (key == "transforms") {
            //parse_transforms(vnode, doc, primitive);
        }
        // scene->primitives.push_back(...)
    }
}

void parse_geometry(yaml_node_t *node, yaml_document_t *doc, Primitive *primitive) {
    assert(node->type == YAML_MAPPING_NODE);
}

void parse_material(yaml_node_t *node, yaml_document_t *doc, Primitive *primitive) {
    assert(node->type == YAML_MAPPING_NODE);
}

void parse_transforms(yaml_node_t *node, yaml_document_t *doc, Primitive *primitive) {
    assert(node->type == YAML_SEQUENCE_NODE);
}

void parse_camera(yaml_node_t *node, yaml_document_t *doc, scene_t *scene) {
    assert(node->type == YAML_MAPPING_NODE);
    auto mapping = node->data.mapping;
    for (auto *item = mapping.pairs.start; item != mapping.pairs.top; item++) {
        auto knode = yaml_document_get_node(doc, item->key);
        auto vnode = yaml_document_get_node(doc, item->value);
        string key = get_scalar<string>(knode);
        if (key == "position") {
            get_sequence<float, 3>(vnode, doc, scene->camera.position);
        } else if (key == "direction") {
            get_sequence<float, 3>(vnode, doc, scene->camera.direction);
        }
    }
}

void parse_settings(yaml_node_t *node, yaml_document_t *doc, scene_t *scene) {
    assert(node->type == YAML_MAPPING_NODE);
    auto mapping = node->data.mapping;
    for (auto *item = mapping.pairs.start; item != mapping.pairs.top; item++) {
        auto knode = yaml_document_get_node(doc, item->key);
        auto vnode = yaml_document_get_node(doc, item->value);
        string key = get_scalar<string>(knode);
        if (key == "frame") {
            get_sequence<int, 4>(vnode, doc, scene->settings.frame);
        }
    }
}

template <typename T>
T get_scalar(yaml_node_t *node) {
    assert(node->type == YAML_SCALAR_NODE);
    return T(node->data.scalar.value, node->data.scalar.value + node->data.scalar.length);
}

template <typename T, int size>
void get_sequence(yaml_node_t *node, yaml_document_t *doc, T *values) {
    assert(node->type == YAML_SEQUENCE_NODE);
    auto seq = node->data.sequence;
    for (auto *item = seq.items.start; item != seq.items.top; item++) {
        auto inode = yaml_document_get_node(doc, *item);
        string val = get_scalar<string>(inode);
        cout << "get_sequence: got scalar " << val << endl;
    }
}
