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
    yaml_parser_load(&_parser, &_document);
    yaml_node_t *root = yaml_document_get_root_node(&_document);
    shared_ptr<Scene> scene = nullptr;
    if (root) {
        scene = parse_scene(root);
    }
    _transforms.clear();
    _geometries.clear();
    _materials.clear();
    yaml_document_delete(&_document);
    return scene;
}

shared_ptr<Scene> Parser::parse_scene(yaml_node_t *node) {
    assert(node->type == YAML_MAPPING_NODE);
    shared_ptr<Aggregate> aggregate = nullptr;
    shared_ptr<Camera> camera = nullptr;
    shared_ptr<Renderer> renderer = nullptr;
    _traverse_mapping(node, [this, &aggregate, &camera, &renderer](string &key, yaml_node_t *value) {
        if (key == "primitives") {
            aggregate = parse_primitives(value);
        } else if (key == "camera") {
            camera = parse_camera(value);
        } else if (key == "renderer") {
            renderer = parse_renderer(value);
        }
    });
    return make_shared<Scene>(aggregate, camera, renderer);
}

shared_ptr<Aggregate> Parser::parse_primitives(yaml_node_t *node) {
    vector<Primitive> primitives;
    _traverse_sequence(node, [this, &primitives](yaml_node_t *item) {
        primitives.push_back(parse_primitive(item));
    });
    return make_shared<Aggregate>(primitives);
}

Primitive Parser::parse_primitive(yaml_node_t *node) {
    shared_ptr<Geometry> geometry = nullptr;
    shared_ptr<Material> material = nullptr;
    shared_ptr<Transform> transform = nullptr;
    _traverse_mapping(node, [this, &geometry, &material, &transform](string &key, yaml_node_t *value) {
        if (key == "geometry") {
            geometry = parse_geometry(value);
        } else if (key == "material") {
            material = parse_material(value);
        } else if (key == "transform") {
            transform = parse_transform(value);
        }
    });
    Transform inv = inverse(*transform);
    return Primitive(geometry, transform, make_shared<Transform>(inv));
}

shared_ptr<Geometry> Parser::parse_geometry(yaml_node_t *node) {
    int index = node - _document.nodes.start;
    auto cache = _geometries.find(index);
    if (cache != _geometries.end()) {
        return cache->second;
    }

    shared_ptr<Geometry> geometry = nullptr;
    string tag((char *)node->tag);
    if (tag == "!mesh") {
        string url;
        _traverse_mapping(node, [this, &url](string &key, yaml_node_t *value) {
            if (key == "url") {
                url = _get_scalar<string>(value);
            }
        });
        if (file_exists(url + ".mesh") && file_exists(url + ".kdtree")) {
            geometry = Mesh::from_cache_file(url.c_str());
        } else {
            geometry = Mesh::from_obj_file(url.c_str());
        }
    } else if (tag == "!sphere") {
        float radius = 1.0;
        _traverse_mapping(node, [this, &radius](string &key, yaml_node_t *value) {
            if (key == "radius") {
                radius = _get_scalar<float>(value);
            }
        });
        geometry = make_shared<Sphere>(radius);
    } else if (tag == "!plane") {
        geometry = make_shared<Plane>();
    }

    if (geometry) {
        _geometries.insert(pair<int, shared_ptr<Geometry>>(index, geometry));
        return geometry;
    } else {
        throw std::runtime_error("unknown geometry type");
    }
}

shared_ptr<Material> Parser::parse_material(yaml_node_t *node) {
    int index = node - _document.nodes.start;
    auto cache = _materials.find(index);
    if (cache != _materials.end()) {
        return cache->second;
    }

    assert(node->type == YAML_MAPPING_NODE);
    auto material = make_shared<Material>();

    if (material) {
        _materials.insert(pair<int, shared_ptr<Material>>(index, material));
        return material;
    } else {
        throw std::runtime_error("unknown material type");
    }
}

shared_ptr<Transform> Parser::parse_transform(yaml_node_t *node) {
    int index = node - _document.nodes.start;
    auto cache = _transforms.find(index);
    if (cache != _transforms.end()) {
        return cache->second;
    }

    shared_ptr<Transform> transform = nullptr;

    if (node->type == YAML_SEQUENCE_NODE) {
        vector<shared_ptr<Transform>> transforms;
        _traverse_sequence(node, [this, &transforms](yaml_node_t *item) {
            transforms.push_back(parse_transform(item));
        });
        transform = Transform::compose(transforms);
    } else {
        string tag((char *)node->tag);
        if (tag == "!translate") {
            Vector delta(0.0, 0.0, 0.0);
            _traverse_mapping(node, [this, &delta](string &key, yaml_node_t *value) {
                if (key == "delta") {
                    auto seq = _get_sequence<float, 3>(value);
                    delta = Vector(&seq[0]);
                }
            });
            transform = Transform::translate(delta);
        } else if (tag == "!scale") {
            Vector coefs(1.0, 1.0, 1.0);
            _traverse_mapping(node, [this, &coefs](string &key, yaml_node_t *value) {
                if (key == "coefs") {
                    auto seq = _get_sequence<float, 3>(value);
                    coefs = Vector(&seq[0]);
                }
            });
            transform = Transform::scale(coefs);
        } else if (tag == "!rotate") {
            Vector axis(0.0, 1.0, 0.0);
            float angle = 0.0;
            _traverse_mapping(node, [this, &axis, &angle](string &key, yaml_node_t *value) {
                if (key == "axis") {
                    auto seq = _get_sequence<float, 3>(value);
                    axis = Vector(&seq[0]);
                } else if (key == "angle") {
                    angle = _get_scalar<float>(value);
                }
            });
            transform = Transform::rotate(axis, angle);
        } else if (tag == "!look_at") {
            Vector position(0.0, 0.0, 0.0);
            Vector target(0.0, 0.0, 1.0);
            Vector up(0.0, 1.0, 0.0);
            _traverse_mapping(node, [this, &position, &target, &up](string &key, yaml_node_t *value) {
                if (key == "position") {
                    auto seq = _get_sequence<float, 3>(value);
                    position = Vector(&seq[0]);
                } else if (key == "target") {
                    auto seq = _get_sequence<float, 3>(value);
                    target = Vector(&seq[0]);
                } else if (key == "up") {
                    auto seq = _get_sequence<float, 3>(value);
                    up = Vector(&seq[0]);
                }
            });
            transform = Transform::look_at(position, target, up);
        } else if (tag == "!identity") {
            transform = make_shared<Transform>();
        }
    }

    if (transform) {
        _transforms.insert(pair<int, shared_ptr<Transform>>(index, transform));
        return transform;
    } else {
        throw std::runtime_error("unknown transform type");
    }
}

shared_ptr<Camera> Parser::parse_camera(yaml_node_t *node) {
    string tag((char *)node->tag);
    if (tag == "!perspective") {
        shared_ptr<Transform> transform = nullptr;
        shared_ptr<Film> film = nullptr;
        float field_of_view = 60.0;
        float lens_radius = 1.0;
        float focal_distance = 100.0;
        _traverse_mapping(node, [this, &transform, &film, &field_of_view, &lens_radius, &focal_distance](string &key, yaml_node_t *value) {
            if (key == "transform") {
                transform = parse_transform(value);
            } else if (key == "field_of_view") {
                field_of_view = _get_scalar<float>(value);
            } else if (key == "lens_radius") {
                lens_radius = _get_scalar<float>(value);
            } else if (key == "focal_distance") {
                focal_distance = _get_scalar<float>(value);
            } else if (key == "film") {
                film = parse_film(value);
            }
        });
        return make_shared<PerspectiveCamera>(transform, film, field_of_view, lens_radius, focal_distance);
    }
    throw std::runtime_error("unknown camera type");
}

shared_ptr<Film> Parser::parse_film(yaml_node_t *node) {
    int xres = 0, yres = 0;
    _traverse_mapping(node, [this, &xres, &yres](string &key, yaml_node_t *value) {
        if (key == "resolution") {
            auto seq = _get_sequence<int, 2>(value);
            xres = seq[0];
            yres = seq[1];
        }
    });
    return make_shared<Film>(xres, yres);
}

shared_ptr<Renderer> Parser::parse_renderer(yaml_node_t *node) {
    string tag((char *)node->tag);
    if (tag == "!sampler") {
        int samples_per_pixel = 1;
        int thread_tiles[2] = {1, 1};
        _traverse_mapping(node, [this, &samples_per_pixel, &thread_tiles](string &key, yaml_node_t *value) {
            if (key == "samples_per_pixel") {
                samples_per_pixel = _get_scalar<int>(value);
            } else if (key == "thread_tiles") {
                auto seq = _get_sequence<int, 2>(value);
                thread_tiles[0] = seq[0];
                thread_tiles[1] = seq[1];
            }
        });
        return make_shared<Sampler>(samples_per_pixel, thread_tiles);
    }
    throw std::runtime_error("unknown renderer type");
}

void Parser::_traverse_mapping(yaml_node_t *node, function<void(std::string&, yaml_node_t*)> func) {
    assert(node->type == YAML_MAPPING_NODE);
    auto map = node->data.mapping;
    for (auto *item = map.pairs.start; item != map.pairs.top; ++item) {
        auto knode = yaml_document_get_node(&_document, item->key);
        auto vnode = yaml_document_get_node(&_document, item->value);
        string key = _get_scalar<string>(knode);
        func(key, vnode);
    }
}

void Parser::_traverse_sequence(yaml_node_t *node, function<void(yaml_node_t*)> func) {
    assert(node->type == YAML_SEQUENCE_NODE);
    auto seq = node->data.sequence;
    for (auto *item = seq.items.start; item != seq.items.top; ++item) {
        auto inode = yaml_document_get_node(&_document, *item);
        func(inode);
    }
}

template <typename T>
T Parser::_get_scalar(yaml_node_t *node) {
    assert(node->type == YAML_SCALAR_NODE);
    string str(node->data.scalar.value, node->data.scalar.value + node->data.scalar.length);
    stringstream stream(str);
    T t;
    stream >> t;
    return t;
}

template <typename T, int size>
vector<T> Parser::_get_sequence(yaml_node_t *node) {
    assert(node->type == YAML_SEQUENCE_NODE);
    vector<T> output;
    auto seq = node->data.sequence;
    for (auto *item = seq.items.start; item != seq.items.top; ++item) {
        auto inode = yaml_document_get_node(&_document, *item);
        output.push_back(_get_scalar<T>(inode));
    }
    return output;
}

}}
