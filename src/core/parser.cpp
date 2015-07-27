#include <cassert>
#include <sstream>
#include <stdexcept>

#include "core/parser.h"
#include "geometry/mesh.h"
#include "geometry/sphere.h"
#include "geometry/plane.h"
#include "camera/perspective.h"

namespace gill { namespace core {

using namespace std;
using namespace gill::geometry;
using namespace gill::camera;

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

bool file_exists(const string &filename) {
    auto f = fopen(filename.c_str(), "r");
    if (f) {
        fclose(f);
        return true;
    } else {
        return false;
    }
}

Parser::Parser() {
    yaml_parser_initialize(&_parser);
    yaml_parser_set_input_file(&_parser, stdin);
}

Parser::Parser(const char *filename) {
    yaml_parser_initialize(&_parser);
    _input = fopen(filename, "r");
    yaml_parser_set_input_file(&_parser, _input);
}

Parser::~Parser() {
    yaml_parser_delete(&_parser);
    if (_input) {
        fclose(_input);
    }
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
    shared_ptr<Camera> camera = nullptr;
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
            //settings = parse_settings(doc, vnode);
        }
    }
    return make_shared<Scene>(primitives, camera);
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
    shared_ptr<Geometry> geometry = nullptr;
    shared_ptr<Material> material = nullptr;
    shared_ptr<Transform> transform = nullptr;
    auto map = node->data.mapping;
    for (auto *item = map.pairs.start; item != map.pairs.top; item++) {
        auto knode = yaml_document_get_node(doc, item->key);
        auto vnode = yaml_document_get_node(doc, item->value);
        string key = get_scalar<string>(knode);
        if (key == "geometry") {
            geometry = parse_geometry(doc, vnode);
        } else if (key == "material") {
            material = parse_material(doc, vnode);
        } else if (key == "transform") {
            transform = parse_transform(doc, vnode);
        }
    }
    Transform inv = inverse(*transform);
    return Primitive(geometry, transform, make_shared<Transform>(inv));
}

shared_ptr<Geometry> Parser::parse_geometry(yaml_document_t *doc, yaml_node_t *node) {
    string tag((char *)node->tag);
    if (tag == "!mesh") {
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
        if (file_exists(url + ".mesh") && file_exists(url + ".kdtree")) {
            return Mesh::from_cache_file(url.c_str());
        } else {
            return Mesh::from_obj_file(url.c_str());
        }
    } else if (tag == "!sphere") {
        assert(node->type == YAML_MAPPING_NODE);
        float radius = 1.0;
        auto map = node->data.mapping;
        for (auto *item = map.pairs.start; item != map.pairs.top; item++) {
            auto knode = yaml_document_get_node(doc, item->key);
            auto vnode = yaml_document_get_node(doc, item->value);
            string key = get_scalar<string>(knode);
            if (key == "radius") {
                radius = get_scalar<float>(vnode);
            }
        }
        return make_shared<Sphere>(radius);
    } else if (tag == "!plane") {
        return make_shared<Plane>();
    } else {
        return make_shared<Sphere>(0.0);
    }
}

shared_ptr<Material> Parser::parse_material(yaml_document_t *doc, yaml_node_t *node) {
    assert(node->type == YAML_MAPPING_NODE);
    return make_shared<Material>();
}

shared_ptr<Transform> Parser::parse_transform(yaml_document_t *doc, yaml_node_t *node) {
    if (node->type == YAML_SEQUENCE_NODE) {
        vector<shared_ptr<Transform>> transforms;
        auto seq = node->data.sequence;
        for (auto *item = seq.items.start; item != seq.items.top; item++) {
            auto inode = yaml_document_get_node(doc, *item);
            transforms.push_back(parse_transform(doc, inode));
        }
        return Transform::compose(transforms);
    } else {
        string tag((char *)node->tag);
        if (tag == "!translate") {
            Vector delta(0.0, 0.0, 0.0);
            auto map = node->data.mapping;
            for (auto *item = map.pairs.start; item != map.pairs.top; item++) {
                auto knode = yaml_document_get_node(doc, item->key);
                auto vnode = yaml_document_get_node(doc, item->value);
                string key = get_scalar<string>(knode);
                if (key == "delta") {
                    auto seq = get_sequence<float, 3>(doc, vnode);
                    delta = Vector(&seq[0]);
                }
            }
            return Transform::translate(delta);
        } else if (tag == "!scale") {
            Vector coefs(1.0, 1.0, 1.0);
            auto map = node->data.mapping;
            for (auto *item = map.pairs.start; item != map.pairs.top; item++) {
                auto knode = yaml_document_get_node(doc, item->key);
                auto vnode = yaml_document_get_node(doc, item->value);
                string key = get_scalar<string>(knode);
                if (key == "coefs") {
                    auto seq = get_sequence<float, 3>(doc, vnode);
                    coefs = Vector(&seq[0]);
                }
            }
            return Transform::scale(coefs);
        } else if (tag == "!rotate") {
            Vector axis(0.0, 1.0, 0.0);
            float angle = 0.0;
            auto map = node->data.mapping;
            for (auto *item = map.pairs.start; item != map.pairs.top; item++) {
                auto knode = yaml_document_get_node(doc, item->key);
                auto vnode = yaml_document_get_node(doc, item->value);
                string key = get_scalar<string>(knode);
                if (key == "axis") {
                    auto seq = get_sequence<float, 3>(doc, vnode);
                    axis = Vector(&seq[0]);
                } else if (key == "angle") {
                    angle = get_scalar<float>(vnode);
                }
            }
            return Transform::rotate(axis, angle);
        } else if (tag == "!look_at") {
            Vector position(0.0, 0.0, 0.0);
            Vector target(0.0, 0.0, 1.0);
            Vector up(0.0, 1.0, 0.0);
            auto map = node->data.mapping;
            for (auto *item = map.pairs.start; item != map.pairs.top; item++) {
                auto knode = yaml_document_get_node(doc, item->key);
                auto vnode = yaml_document_get_node(doc, item->value);
                string key = get_scalar<string>(knode);
                if (key == "position") {
                    auto seq = get_sequence<float, 3>(doc, vnode);
                    position = Vector(&seq[0]);
                } else if (key == "target") {
                    auto seq = get_sequence<float, 3>(doc, vnode);
                    target = Vector(&seq[0]);
                } else if (key == "up") {
                    auto seq = get_sequence<float, 3>(doc, vnode);
                    up = Vector(&seq[0]);
                }
            }
            return Transform::look_at(position, target, up);
        } else if (tag == "!identity") {
            return make_shared<Transform>();
        }
        return make_shared<Transform>();
    }
}

shared_ptr<Camera> Parser::parse_camera(yaml_document_t *doc, yaml_node_t *node) {
    string tag((char *)node->tag);
    if (tag == "!perspective") {
        assert(node->type == YAML_MAPPING_NODE);
        shared_ptr<Transform> transform = nullptr;
        shared_ptr<Film> film = nullptr;
        float field_of_view = 60.0;
        float lens_radius = 1.0;
        float focal_distance = 100.0;
        auto map = node->data.mapping;
        for (auto *item = map.pairs.start; item != map.pairs.top; item++) {
            auto knode = yaml_document_get_node(doc, item->key);
            auto vnode = yaml_document_get_node(doc, item->value);
            string key = get_scalar<string>(knode);
            if (key == "transform") {
                transform = parse_transform(doc, vnode);
            } else if (key == "field_of_view") {
                field_of_view = get_scalar<float>(vnode);
            } else if (key == "lens_radius") {
                lens_radius = get_scalar<float>(vnode);
            } else if (key == "focal_distance") {
                focal_distance = get_scalar<float>(vnode);
            } else if (key == "film") {
                film = parse_film(doc, vnode);
            }
        }
        return make_shared<PerspectiveCamera>(transform, film, field_of_view, lens_radius, focal_distance);
    }
    throw std::runtime_error("unknown camera type");
}

shared_ptr<Film> Parser::parse_film(yaml_document_t *doc, yaml_node_t *node) {
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
    return make_shared<Film>(xres, yres);
}

/*
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
*/

}}
