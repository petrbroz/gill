#include <cassert>
#include <sstream>
#include "parser.h"

namespace gill {

using namespace std;
using namespace core;

template <typename T>
T get_scalar(yaml_node_t *node) {
    assert(node->type == YAML_SCALAR_NODE);
    string str(node->data.scalar.value, node->data.scalar.value + node->data.scalar.length);
    stringstream stream(str);
    T t;
    stream >> t;
    return t;
}

template <typename T, int size>
vector<T> get_sequence(yaml_document_t *doc, yaml_node_t *node) {
    assert(node->type == YAML_SEQUENCE_NODE);
    vector<T> output;
    auto seq = node->data.sequence;
    for (auto *item = seq.items.start; item != seq.items.top; item++) {
        auto inode = yaml_document_get_node(doc, *item);
        output.push_back(get_scalar<T>(inode));
    }
    return output;
}

Parser::Parser() {
    yaml_parser_initialize(&_parser);
    yaml_parser_set_input_file(&_parser, stdin);
}

Parser::~Parser() {
    yaml_parser_delete(&_parser);
}

shared_ptr<Scene> Parser::next_scene() {
    yaml_document_t document;
    yaml_parser_load(&_parser, &document);
    yaml_node_t *root = yaml_document_get_root_node(&document);
    shared_ptr<Scene> scene = nullptr;
    if (root) {
        scene = parse_scene(&document, root);
    }
    yaml_document_delete(&document);
    return scene;
}

shared_ptr<Scene> Parser::parse_scene(yaml_document_t *doc, yaml_node_t *node) {
    assert(node->type == YAML_MAPPING_NODE);
    vector<Primitive> primitives;
    Camera camera(Point(0.0), Point(0.0), 90.0, Film(0, 0));
    Scene::Settings settings;
    auto map = node->data.mapping;
    for (auto *item = map.pairs.start; item != map.pairs.top; item++) {
        auto knode = yaml_document_get_node(doc, item->key);
        auto vnode = yaml_document_get_node(doc, item->value);
        string key = get_scalar<string>(knode);
        if (key == "primitives") {
            primitives = parse_primitives(doc, vnode);
        } else if (key == "camera") {
            camera = parse_camera(doc, vnode);
        } else if (key == "settings") {
            settings = parse_settings(doc, vnode);
        }
    }
    return make_shared<Scene>(primitives, camera, settings);
}

vector<Primitive> Parser::parse_primitives(yaml_document_t *doc, yaml_node_t *node) {
    assert(node->type == YAML_SEQUENCE_NODE);
    vector<Primitive> primitives;
    auto seq = node->data.sequence;
    for (auto *item = seq.items.start; item != seq.items.top; item++) {
        auto inode = yaml_document_get_node(doc, *item);
        primitives.push_back(parse_primitive(doc, inode));
    }
    return primitives;
}

Primitive Parser::parse_primitive(yaml_document_t *doc, yaml_node_t *node) {
    assert(node->type == YAML_MAPPING_NODE);
    shared_ptr<Mesh> mesh = nullptr;
    shared_ptr<Material> material = nullptr;
    shared_ptr<Transform> transform = nullptr;
    auto map = node->data.mapping;
    for (auto *item = map.pairs.start; item != map.pairs.top; item++) {
        auto knode = yaml_document_get_node(doc, item->key);
        auto vnode = yaml_document_get_node(doc, item->value);
        string key = get_scalar<string>(knode);
        if (key == "geometry") {
            mesh = parse_geometry(doc, vnode);
        } else if (key == "material") {
            material = parse_material(doc, vnode);
        } else if (key == "transform") {
            transform = parse_transform(doc, vnode);
        }
    }
    Transform inv = inverse(*transform);
    return Primitive(mesh, transform, make_shared<Transform>(inv));
}

shared_ptr<Mesh> Parser::parse_geometry(yaml_document_t *doc, yaml_node_t *node) {
    assert(node->type == YAML_MAPPING_NODE);
    string url;
    auto map = node->data.mapping;
    for (auto *item = map.pairs.start; item != map.pairs.top; item++) {
        auto knode = yaml_document_get_node(doc, item->key);
        auto vnode = yaml_document_get_node(doc, item->value);
        string key = get_scalar<string>(knode);
        if (key == "url") {
            url = get_scalar<string>(vnode);
        }
    }
    return Mesh::from_obj_file(url.data());
}

shared_ptr<Material> Parser::parse_material(yaml_document_t *doc, yaml_node_t *node) {
    assert(node->type == YAML_MAPPING_NODE);
    return make_shared<Material>();
}

shared_ptr<Transform> Parser::parse_transform(yaml_document_t *doc, yaml_node_t *node) {
    assert(node->type == YAML_MAPPING_NODE);
    return Transform::translate(0.0, 0.0, 0.0);
}

Camera Parser::parse_camera(yaml_document_t *doc, yaml_node_t *node) {
    assert(node->type == YAML_MAPPING_NODE);
    Point position;
    Point look_at;
    float fov;
    Film film(0, 0);
    auto map = node->data.mapping;
    for (auto *item = map.pairs.start; item != map.pairs.top; item++) {
        auto knode = yaml_document_get_node(doc, item->key);
        auto vnode = yaml_document_get_node(doc, item->value);
        string key = get_scalar<string>(knode);
        if (key == "position") {
            auto seq = get_sequence<float, 3>(doc, vnode);
            position = Point(&seq[0]);
        } else if (key == "look_at") {
            auto seq = get_sequence<float, 3>(doc, vnode);
            look_at = Point(&seq[0]);
        } else if (key == "field_of_view") {
            fov = get_scalar<float>(vnode);
        } else if (key == "film") {
            film = parse_film(doc, vnode);
        }
    }
    return Camera(position, look_at, fov, film);
}

Film Parser::parse_film(yaml_document_t *doc, yaml_node_t *node) {
    assert(node->type == YAML_MAPPING_NODE);
    int xres = 0, yres = 0;
    auto map = node->data.mapping;
    for (auto *item = map.pairs.start; item != map.pairs.top; item++) {
        auto knode = yaml_document_get_node(doc, item->key);
        auto vnode = yaml_document_get_node(doc, item->value);
        string key = get_scalar<string>(knode);
        if (key == "resolution") {
            auto seq = get_sequence<int, 2>(doc, vnode);
            xres = seq[0];
            yres = seq[1];
        }
    }
    return Film(xres, yres);
}

Scene::Settings Parser::parse_settings(yaml_document_t *doc, yaml_node_t *node) {
    assert(node->type == YAML_MAPPING_NODE);
    Scene::Settings settings;
    settings.frame[0] = -128;
    settings.frame[1] = -128;
    settings.frame[2] = +128;
    settings.frame[3] = +128;
    auto map = node->data.mapping;
    for (auto *item = map.pairs.start; item != map.pairs.top; item++) {
        auto knode = yaml_document_get_node(doc, item->key);
        auto vnode = yaml_document_get_node(doc, item->value);
        string key = get_scalar<string>(knode);
        if (key == "frame") {
            auto seq = get_sequence<int, 4>(doc, vnode);
            copy(seq.begin(), seq.end(), settings.frame);
        }
    }
    return settings;
}

}
