#ifndef GILL_CORE_KDTREE_H_
#define GILL_CORE_KDTREE_H_

#include <iostream>
#include <vector>
#include <cstdint>
#include <cmath>
#include <algorithm>
#include <functional>

#include "core/bbox.h"
#include "core/math.h"
#include "core/ray.h"
#include "core/vector.h"

namespace gill { namespace core {

const int MaxTreeSegments = 64;
const int KdTreeFileMagic = 0xacc1;

/**
 * Generic kD-tree for accelerating ray-to-geometry intersection tests.
 * Geometry must be a type with two methods:
 * - bounds() - returning the geometry bounds in kD-tree's coordinate system.
 * - intersect(ray, t, intersection) - computing an intersection of the geometry with a ray.
 */
template <typename Geom>
class KdTree {

    typedef typename Geom::Intersection Intersection;

    /**
     * Compact 8-byte representation of a kD-tree node.
     * The 2 least significant bits of 'header' define the split axis (0 = X, 1 = Y, 2 = Z, 3 = none/leaf).
     * For leaves, the higher 30 bits of 'header' define index into a list of geometries
     * that overlap the leaf volume (and 'geom_count' defines their count).
     * For internal nodes, the higher 30 bits of 'header' define index to the front child node
     * (and 'split' defines the offset of the splitting plane).
     */
    struct Node {
        uint32_t header;
        union {
            float split;
            uint32_t geom_count;
        };

        bool is_leaf() const {
            return (header & 3) == 3;
        }

        int split_axis() const {
            return header & 3;
        }

        int front_child_offset() const {
            return header >> 2;
        }
    };

    /**
     * Representation of either the minimum or the maximum of a geometry bounds in certain direction.
     * Used for sorting the boundaries when computing the ideal split plane.
     */
    struct Edge {
        float split;
        uint32_t index;
        enum { START, END } type;

        Edge() { }

        Edge(float _split, uint32_t _index, bool start) {
            split = _split;
            index = _index;
            type = start ? START : END;
        }

        bool operator<(const Edge &e) const {
            if (split == e.split) {
                return (int)type < (int)e.type;
            } else {
                return split < e.split;
            }
        }
    };

    /**
     * Current node and ray range during a kD-tree traversal.
     */
    struct TreeSegment {
        Node *node;
        float tmin, tmax;
    };

    Node leaf_node(uint32_t *overlapping, int num_overlapping) {
        int index = _geom_refs.size();
        _geom_refs.insert(_geom_refs.end(), overlapping, overlapping + num_overlapping);
        Node n;
        n.header = 3 | (index << 2);
        n.geom_count = num_overlapping;
        return n;
    }

    Node internal_node(int axis, float split, int above_child_index) {
        Node n;
        n.header = axis | (above_child_index << 2);
        n.split = split;
        return n;
    }

    float _isec_cost; /// The computation cost of intersecting a ray with one geometry
    float _trav_cost; /// The computation cost of traversing children of a kD-tree node
    int _max_geoms; /// Max. number of geometries allowed in a leaf node.
    int _max_depth; /// Max. allowed depth of the kD-tree.
    BBox _total_bounds;
    std::vector<Node> _nodes;
    std::vector<uint32_t> _geom_refs;
    std::function<BBox(uint32_t)> _bounds_func;
    std::function<bool(uint32_t, const Ray&, float&, Intersection*)> _isec_func;

    int build(const BBox &node_bounds, BBox *geom_bounds, Edge *edges[3],
            uint32_t *overlapping, int num_overlapping, uint32_t *below, uint32_t *above, int depth) {
        int node_index = _nodes.size();
        // If termination condition was reached, create a leaf node and return
        if (num_overlapping <= _max_geoms || depth == _max_depth) {
            _nodes.push_back(leaf_node(overlapping, num_overlapping));
            return node_index;
        }

        int best_axis = -1, best_edge = -1;
        float best_cost = Infinity, curr_cost = _isec_cost * num_overlapping;
        float inv_total_surf = 1.0 / surface(node_bounds);
        int num_below, num_above;
        Vector diagonal = node_bounds.max - node_bounds.min;
        int axis = 0, axis2, axis3;
        if (diagonal.y > diagonal.x && diagonal.y > diagonal.z) {
            axis = 1;
        } else if (diagonal.z > diagonal.x && diagonal.z > diagonal.y) {
            axis = 2;
        }

        for (int attempt = 0; attempt < 3; ++attempt) {
            axis2 = (axis + 1) % 3;
            axis3 = (axis + 2) % 3;

            // Prepare the edges for the current axis
            for (int i = 0; i < num_overlapping; ++i) {
                uint32_t index = overlapping[i];
                const BBox &bounds = geom_bounds[index];
                edges[axis][2 * i] = Edge(bounds.min[axis], index, true);
                edges[axis][2 * i + 1] = Edge(bounds.max[axis], index, false);
            }
            std::sort(&edges[axis][0], &edges[axis][2 * num_overlapping]);

            // Try to find a better split for the current axis
            num_below = 0;
            num_above = num_overlapping;
            for (int i = 0; i < 2 * num_overlapping; ++i) {
                if (edges[axis][i].type == Edge::END) {
                    --num_above;
                }
                float split = edges[axis][i].split;
                if (split > node_bounds.min[axis] && split < node_bounds.max[axis]) {
                    float surf_below = 2.0 * (diagonal[axis2] * diagonal[axis3] + (split - node_bounds.min[axis]) * (diagonal[axis2] + diagonal[axis3]));
                    float prob_below = surf_below * inv_total_surf;
                    float surf_above = 2.0 * (diagonal[axis2] * diagonal[axis3] + (node_bounds.max[axis] - split) * (diagonal[axis2] + diagonal[axis3]));
                    float prob_above = surf_above * inv_total_surf;
                    float cost = _trav_cost + _isec_cost * (prob_below * num_below + prob_above * num_above);
                    if (cost < best_cost) {
                        best_cost = cost;
                        best_axis = axis;
                        best_edge = i;
                    }
                }
                if (edges[axis][i].type == Edge::START) {
                    ++num_below;
                }
            }

            if (best_cost < curr_cost) {
                break;
            }

            axis = (axis + 1) % 3;
        }

        // Give up if no better split was found
        if (curr_cost < best_cost) {
            _nodes.push_back(leaf_node(overlapping, num_overlapping));
            return node_index;
        }

        // Classify geometries below/above the found split
        num_below = num_above = 0;
        for (int i = 0; i < best_edge; ++i) {
            Edge e = edges[best_axis][i];
            if (e.type == Edge::START) {
                below[num_below++] = e.index;
            }
        }
        for (int i = best_edge + 1; i < 2 * num_overlapping; ++i) {
            Edge e = edges[best_axis][i];
            if (e.type == Edge::END) {
                above[num_above++] = e.index;
            }
        }

        float split = edges[best_axis][best_edge].split;
        BBox bounds_below = node_bounds;
        bounds_below.max[best_axis] = split;
        BBox bounds_above = node_bounds;
        bounds_above.min[best_axis] = split;
        _nodes.push_back(Node());
        /*int below_child_idx =*/ build(bounds_below, geom_bounds, edges,
                below, num_below, below, above + num_overlapping, depth + 1);
        int above_child_idx = build(bounds_above, geom_bounds, edges,
                above, num_above, below, above + num_overlapping, depth + 1);
        _nodes[node_index] = internal_node(best_axis, split, above_child_idx);
        return node_index;
    }

public:
    KdTree(uint32_t geom_count, float isec_cost, float trav_cost, int max_geoms, int max_depth,
            std::function<BBox(uint32_t)> bounds_func, std::function<bool(uint32_t, const Ray&, float&, Intersection*)> isec_func)
        : _isec_cost(isec_cost), _trav_cost(trav_cost), _max_geoms(max_geoms), _max_depth(max_depth),
          _bounds_func(bounds_func), _isec_func(isec_func) {
        _nodes.reserve(16);
        BBox *geom_bounds = new BBox[geom_count];
        uint32_t *overlapping = new uint32_t[geom_count];
        uint32_t *below = new uint32_t[geom_count];
        uint32_t *above = new uint32_t[(max_depth + 1) * geom_count];
        for (uint32_t i = 0; i < geom_count; ++i) {
            geom_bounds[i] = _bounds_func(i);
            _total_bounds += geom_bounds[i];
            overlapping[i] = i;
        }

        Edge *edges[3];
        edges[0] = new Edge[2 * geom_count];
        edges[1] = new Edge[2 * geom_count];
        edges[2] = new Edge[2 * geom_count];

        build(_total_bounds, geom_bounds, edges, overlapping, geom_count, below, above, 0);

        delete[] edges[0]; delete[] edges[1]; delete[] edges[2];
        delete[] geom_bounds; delete[] overlapping; delete[] below; delete[] above;
    }

    KdTree(const char *filename,
            std::function<BBox(uint32_t)> bounds_func, std::function<bool(uint32_t, const Ray&, float&, Intersection*)> isec_func)
          : _bounds_func(bounds_func), _isec_func(isec_func) {
        load(filename);
    }

    bool intersect(const Ray &ray, float &t, Intersection *isec) {
        float tmin, tmax;
        if (!_total_bounds.intersects(ray, tmin, tmax)) {
            return false;
        }

        TreeSegment segments[MaxTreeSegments];
        segments[0] = { &_nodes[0], tmin, tmax };
        int num_segments = 1;

        while (num_segments-- > 0) {
            TreeSegment segment = segments[num_segments];
            if (segment.node->is_leaf()) {
                int geom_index = segment.node->header >> 2;
                int geom_count = segment.node->geom_count;
                float old_t = t;
                bool hit = false;
                for (int i = geom_index; i < geom_index + geom_count; ++i) {
                    hit |= _isec_func(_geom_refs[i], ray, t, isec);
                }
                if (hit) {
                    if (t >= segment.tmin && t <= segment.tmax) {
                        return true;
                    } else {
                        t = old_t; // The closest 't' has changed and must be reset
                    }
                }
            } else {
                float split = segment.node->split;
                int split_axis = segment.node->split_axis();
                float ro = ray.o[split_axis], rd = ray.d[split_axis];
                Node *first_child, *last_child;
                if (ro < split || (ro == split && rd < 0.0)) {
                    first_child = segment.node + 1;
                    last_child = &_nodes[0] + segment.node->front_child_offset();
                } else {
                    first_child = &_nodes[0] + segment.node->front_child_offset();
                    last_child = segment.node + 1;
                }

                if (almost_zero(rd)) {
                    segments[num_segments++] = { first_child, segment.tmin, segment.tmax };
                } else {
                    float tsplit = (split - ro) / rd;
                    if (tsplit > segment.tmax || tsplit <= 0.0) {
                        segments[num_segments++] = { first_child, segment.tmin, segment.tmax };
                    } else if (tsplit < segment.tmin) {
                        segments[num_segments++] = { last_child, segment.tmin, segment.tmax };
                    } else {
                        segments[num_segments++] = { last_child, tsplit, segment.tmax };
                        segments[num_segments++] = { first_child, segment.tmin, tsplit };
                    }
                }
            }
        }

        return false;
    }

    BBox bounds() {
        return _total_bounds;
    }

    void print_info() {
        std::cerr << "Intersection Cost: " << _isec_cost << std::endl;
        std::cerr << "Traverse Cost: " << _trav_cost << std::endl;
        std::cerr << "Max Geoms in Leaf: " << _max_geoms << std::endl;
        std::cerr << "Max Tree Depth: " << _max_depth << std::endl;
        std::cerr << "Tree Nodes: " << _nodes.size() << std::endl;
        std::cerr << "Geom Refs: " << _geom_refs.size() << std::endl;
    }

    /**
     * Prints the kD-tree nodes to stderr in graphviz DOT format.
     */
    void print_dot() {
        std::cerr << "digraph G {" << std::endl;
        for (int i = 0; i < _nodes.size(); i++) {
            const Node &n = _nodes[i];
            if (n.is_leaf()) {
                std::cerr << "\tn" << i << " [label=\"geoms " << n.geom_count << "\"];" << std::endl;
            } else {
                std::cerr << "\tn" << i << " [label=\"split " << n.split << "\"];" << std::endl;
                std::cerr << "\tn" << i << " -> n" << (i + 1) << ";" << std::endl;
                std::cerr << "\tn" << i << " -> n" << (n.header >> 2) << ";" << std::endl;
            }
        }
        std::cerr << "}" << std::endl;
    }

    /**
     * Serializes the kD-tree into a binary file.
     * Used for caching purposes.
     * @param filename Name of the output file.
     */
    void save(const char *filename) {
        auto f = fopen(filename, "wb");
        fwrite(&KdTreeFileMagic, sizeof(KdTreeFileMagic), 1, f);
        fwrite(&_isec_cost, sizeof(_isec_cost), 1, f);
        fwrite(&_trav_cost, sizeof(_trav_cost), 1, f);
        fwrite(&_max_geoms, sizeof(_max_geoms), 1, f);
        fwrite(&_max_depth, sizeof(_max_depth), 1, f);
        fwrite(&_total_bounds, sizeof(_total_bounds), 1, f);
        size_t ncount = _nodes.size();
        fwrite(&ncount, sizeof(ncount), 1, f);
        size_t rcount = _geom_refs.size();
        fwrite(&rcount, sizeof(rcount), 1, f);
        fwrite(&_nodes[0], sizeof(Node), ncount, f);
        fwrite(&_geom_refs[0], sizeof(uint32_t), rcount, f);
        fclose(f);
    }

    /**
     * Deserializes the kD-tree from a binary file.
     * Used for caching purposes.
     * @param filename Name of the input file.
     */
    void load(const char *filename) {
        auto f = fopen(filename, "rb");
        int magic;
        fread(&magic, sizeof(KdTreeFileMagic), 1, f);
        if (magic != KdTreeFileMagic) {
            std::cerr << "KdTree::load - incorrect magic number" << std::endl;
            exit(1);
        }
        fread(&_isec_cost, sizeof(_isec_cost), 1, f);
        fread(&_trav_cost, sizeof(_trav_cost), 1, f);
        fread(&_max_geoms, sizeof(_max_geoms), 1, f);
        fread(&_max_depth, sizeof(_max_depth), 1, f);
        fread(&_total_bounds, sizeof(_total_bounds), 1, f);
        size_t ncount, rcount;
        fread(&ncount, sizeof(size_t), 1, f);
        fread(&rcount, sizeof(size_t), 1, f);
        Node *n = new Node[ncount];
        uint32_t *r = new uint32_t[rcount];
        fread(n, sizeof(Node), ncount, f);
        _nodes.clear();
        _nodes.insert(_nodes.begin(), n, n + ncount);
        fread(r, sizeof(uint32_t), rcount, f);
        _geom_refs.clear();
        _geom_refs.insert(_geom_refs.begin(), r, r + rcount);
        delete[] r;
        delete[] n;
        fclose(f);
    }

};

}}

#endif
