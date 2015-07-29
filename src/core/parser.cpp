#include <cassert>
#include <sstream>
#include <stdexcept>

#include "core/parser.h"
#include "geometry/mesh.h"
#include "geometry/sphere.h"
#include "geometry/plane.h"
#include "camera/perspective.h"
#include "renderer/sampler.h"

namespace gill { namespace core {

using namespace std;
using namespace gill::geometry;
using namespace gill::camera;
using namespace gill::renderer;

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
    _geometry_cache.clear();
    _material_cache.clear();
    _transform_cache.clear();
    yaml_document_delete(&document);
    return scene;
}

shared_ptr<Scene> Parser::parse_scene(yaml_document_t *doc, yaml_node_t *node) {
    assert(node->type == YAML_MAPPING_NODE);
    shared_ptr<Aggregate> aggregate = nullptr;
    shared_ptr<Camera> camera = nullptr;
    shared_ptr<Renderer> renderer = nullptr;
    auto map = node->data.mapping;
    for (auto *item = map.pairs.start; item != map.pairs.top; item++) {
        auto knode = yaml_document_get_node(doc, item->key);
        auto vnode = yaml_document_get_node(doc, item->value);
        string key = get_scalar<string>(knode);
        if (key == "primitives") {
            aggregate = parse_primitives(doc, vnode);
        } else if (key == "camera") {
            camera = parse_camera(doc, vnode);
        } else if (key == "renderer") {
            renderer = parse_renderer(doc, vnode);
        }
    }
    return make_shared<Scene>(aggregate, camera, renderer);
}

shared_ptr<Aggregate> Parser::parse_primitives(yaml_document_t *doc, yaml_node_t *node) {
    assert(node->type == YAML_SEQUENCE_NODE);
    vector<Primitive> primitives;
    auto seq = node->data.sequence;
    for (auto *item = seq.items.start; item != seq.items.top; item++) {
        auto inode = yaml_document_get_node(doc, *item);
        primitives.push_back(parse_primitive(doc, inode));
    }
    return make_shared<Aggregate>(primitives);
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
    int index = node - doc->nodes.start;
    auto cache = _geometry_cache.find(index);
    if (cache != _geometry_cache.end()) {
        return cache->second;
    }

    shared_ptr<Geometry> geometry = nullptr;
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
            geometry = Mesh::from_cache_file(url.c_str());
        } else {
            geometry = Mesh::from_obj_file(url.c_str());
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
        geometry = make_shared<Sphere>(radius);
    } else if (tag == "!plane") {
        geometry = make_shared<Plane>();
    }

    if (geometry) {
        _geometry_cache.insert(pair<int, shared_ptr<Geometry>>(index, geometry));
        return geometry;
    } else {
        throw std::runtime_error("unknown geometry type");
    }
}

shared_ptr<Material> Parser::parse_material(yaml_document_t *doc, yaml_node_t *node) {
    int index = node - doc->nodes.start;
    auto cache = _material_cache.find(index);
    if (cache != _material_cache.end()) {
        return cache->second;
    }

    assert(node->type == YAML_MAPPING_NODE);
    auto material = make_shared<Material>();

    if (material) {
        _material_cache.insert(pair<int, shared_ptr<Material>>(index, material));
        return material;
    } else {
        throw std::runtime_error("unknown material type");
    }
}

shared_ptr<Transform> Parser::parse_transform(yaml_document_t *doc, yaml_node_t *node) {
    int index = node - doc->nodes.start;
    auto cache = _transform_cache.find(index);
    if (cache != _transform_cache.end()) {
        return cache->second;
    }

    shared_ptr<Transform> transform = nullptr;

    if (node->type == YAML_SEQUENCE_NODE) {
        vector<shared_ptr<Transform>> transforms;
        auto seq = node->data.sequence;
        for (auto *item = seq.items.start; item != seq.items.top; item++) {
            auto inode = yaml_document_get_node(doc, *item);
            transforms.push_back(parse_transform(doc, inode));
        }
        transform = Transform::compose(transforms);
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
            transform = Transform::translate(delta);
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
            transform = Transform::scale(coefs);
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
            transform = Transform::rotate(axis, angle);
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
            transform = Transform::look_at(position, target, up);
        } else if (tag == "!identity") {
            transform = make_shared<Transform>();
        }
    }

    if (transform) {
        _transform_cache.insert(pair<int, shared_ptr<Transform>>(index, transform));
        return transform;
    } else {
        throw std::runtime_error("unknown transform type");
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

shared_ptr<Renderer> Parser::parse_renderer(yaml_document_t *doc, yaml_node_t *node) {
    string tag((char *)node->tag);
    if (tag == "!sampler") {
        assert(node->type == YAML_MAPPING_NODE);
        int samples_per_pixel = 1;
        int thread_tiles[2] = {1, 1};
        auto map = node->data.mapping;
        for (auto *item = map.pairs.start; item != map.pairs.top; item++) {
            auto knode = yaml_document_get_node(doc, item->key);
            auto vnode = yaml_document_get_node(doc, item->value);
            string key = get_scalar<string>(knode);
            if (key == "samples_per_pixel") {
                samples_per_pixel = get_scalar<int>(vnode);
            } else if (key == "thread_tiles") {
                auto seq = get_sequence<int, 2>(doc, vnode);
                thread_tiles[0] = seq[0];
                thread_tiles[1] = seq[1];
            }
        }
        return make_shared<Sampler>(samples_per_pixel, thread_tiles);
    }
    throw std::runtime_error("unknown renderer type");
}

}}
