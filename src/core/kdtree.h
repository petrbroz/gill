#ifndef GILL_CORE_KDTREE_H_
#define GILL_CORE_KDTREE_H_

#include <iostream>
#include <vector>
#include <cstdint>
#include <cmath>

#include "core/bbox.h"
#include "core/math.h"
#include "core/ray.h"
#include "core/vector.h"

namespace gill { namespace core {

const int MaxTreeSegments = 64;
const int KdTreeFileMagic = 0xacc1;

template <typename Geom>
class KdTree {

    typedef typename Geom::Intersection Intersection;

    /**
     * Compact (8byte) representation of a kd-tree node.
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

    struct TreeSegment {
        Node *node;
        float tmin, tmax;
    };

    Node leaf_node(uint32_t *overlapping, int num_overlapping) {
        int index = geom_refs.size();
        geom_refs.insert(geom_refs.end(), overlapping, overlapping + num_overlapping);
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

    float isec_cost, trav_cost;
    int max_geoms, max_depth;
    BBox total_bounds;
    std::vector<Node> nodes;
    std::vector<uint32_t> geom_refs;

    int build(const BBox &node_bounds, const std::vector<BBox> &geom_bounds, Edge *edges[3],
            uint32_t *overlapping, int num_overlapping, uint32_t *below, uint32_t *above,
            int depth) {
        int node_index = nodes.size();
        // If termination condition was reached, create a leaf node and return
        if (num_overlapping <= max_geoms || depth == max_depth) {
            nodes.push_back(leaf_node(overlapping, num_overlapping));
            return node_index;
        }

        int best_axis = -1, best_edge = -1;
        float best_cost = Infinity, curr_cost = isec_cost * num_overlapping;
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
                    float cost = trav_cost + isec_cost * (prob_below * num_below + prob_above * num_above);
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
            nodes.push_back(leaf_node(overlapping, num_overlapping));
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
        nodes.push_back(Node());
        /*int below_child_idx =*/ build(bounds_below, geom_bounds, edges,
                below, num_below, below, above + num_overlapping, depth + 1);
        int above_child_idx = build(bounds_above, geom_bounds, edges,
                above, num_above, below, above + num_overlapping, depth + 1);
        nodes[node_index] = internal_node(best_axis, split, above_child_idx);
        return node_index;
    }

public:
    KdTree(const std::vector<Geom> &geoms, float _isec_cost, float _trav_cost,
            int _max_geoms, int _max_depth) {
        isec_cost = _isec_cost;
        trav_cost = _trav_cost;
        max_geoms = _max_geoms;
        max_depth = _max_depth;
        nodes.reserve(16);

        std::vector<BBox> geom_bounds;
        geom_bounds.reserve(geoms.size());
        for (const Geom &geom : geoms) {
            BBox bounds = geom.bounds();
            geom_bounds.push_back(bounds);
            total_bounds += bounds;
        }

        uint32_t *overlapping = new uint32_t[geoms.size()];
        for (uint32_t i = 0; i < geoms.size(); ++i) {
            overlapping[i] = i;
        }
        uint32_t *below = new uint32_t[geoms.size()];
        uint32_t *above = new uint32_t[(max_depth + 1) * geoms.size()];

        Edge *edges[3];
        edges[0] = new Edge[2 * geoms.size()];
        edges[1] = new Edge[2 * geoms.size()];
        edges[2] = new Edge[2 * geoms.size()];

        build(total_bounds, geom_bounds, edges, overlapping, geoms.size(), below, above, 0);

        delete[] edges[0]; delete[] edges[1]; delete[] edges[2];
        delete[] overlapping; delete[] below; delete[] above;
    }

    KdTree(const char *filename) {
        load(filename);
    }

    bool intersect(const std::vector<Geom> &geoms, const Ray &ray, float &t, Intersection *isec) {
        float tmin, tmax;
        if (!total_bounds.intersects(ray, tmin, tmax)) {
            return false;
        }

        TreeSegment segments[MaxTreeSegments];
        segments[0] = { &nodes[0], tmin, tmax };
        int num_segments = 1;

        while (num_segments-- > 0) {
            TreeSegment segment = segments[num_segments];
            if (segment.node->is_leaf()) {
                int geom_index = segment.node->header >> 2;
                int geom_count = segment.node->geom_count;
                float _t = Infinity;
                for (int i = geom_index; i < geom_index + geom_count; ++i) {
                    const Geom &geom = geoms[geom_refs[i]];
                    geom.intersect(ray, _t, isec);
                }
                if (_t >= segment.tmin && _t <= segment.tmax) {
                    t = _t;
                    return true;
                }
            } else {
                float split = segment.node->split;
                int split_axis = segment.node->split_axis();
                float ro = ray.o[split_axis], rd = ray.d[split_axis];
                Node *first_child, *last_child;
                if (ro < split || (ro == split && rd < 0.0)) {
                    first_child = segment.node + 1;
                    last_child = &nodes[0] + segment.node->front_child_offset();
                } else {
                    first_child = &nodes[0] + segment.node->front_child_offset();
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
        return total_bounds;
    }

    void print_info() {
        std::cerr << "Intersection Cost: " << isec_cost << std::endl;
        std::cerr << "Traverse Cost: " << trav_cost << std::endl;
        std::cerr << "Max Geoms in Leaf: " << max_geoms << std::endl;
        std::cerr << "Max Tree Depth: " << max_depth << std::endl;
        std::cerr << "Tree Nodes: " << nodes.size() << std::endl;
        std::cerr << "Geom Refs: " << geom_refs.size() << std::endl;
    }

    void print_dot() {
        std::cerr << "digraph G {" << std::endl;
        for (int i = 0; i < nodes.size(); i++) {
            const Node &n = nodes[i];
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

    void save(const char *filename) {
        auto f = fopen(filename, "wb");
        fwrite(&KdTreeFileMagic, sizeof(KdTreeFileMagic), 1, f);
        fwrite(&isec_cost, sizeof(isec_cost), 1, f);
        fwrite(&trav_cost, sizeof(trav_cost), 1, f);
        fwrite(&max_geoms, sizeof(max_geoms), 1, f);
        fwrite(&max_depth, sizeof(max_depth), 1, f);
        fwrite(&total_bounds, sizeof(total_bounds), 1, f);
        size_t ncount = nodes.size();
        fwrite(&ncount, sizeof(ncount), 1, f);
        size_t rcount = geom_refs.size();
        fwrite(&rcount, sizeof(rcount), 1, f);
        fwrite(&nodes[0], sizeof(Node), ncount, f);
        fwrite(&geom_refs[0], sizeof(uint32_t), rcount, f);

        fclose(f);
    }

    void load(const char *filename) {
        auto f = fopen(filename, "rb");
        int magic;
        fread(&magic, sizeof(KdTreeFileMagic), 1, f);
        if (magic != KdTreeFileMagic) {
            std::cerr << "KdTree::load - incorrect magic number" << std::endl;
            exit(1);
        }
        fread(&isec_cost, sizeof(isec_cost), 1, f);
        fread(&trav_cost, sizeof(trav_cost), 1, f);
        fread(&max_geoms, sizeof(max_geoms), 1, f);
        fread(&max_depth, sizeof(max_depth), 1, f);
        fread(&total_bounds, sizeof(total_bounds), 1, f);
        size_t ncount, rcount;
        fread(&ncount, sizeof(size_t), 1, f);
        fread(&rcount, sizeof(size_t), 1, f);
        Node *n = new Node[ncount];
        uint32_t *r = new uint32_t[rcount];
        fread(n, sizeof(Node), ncount, f);
        nodes.clear();
        nodes.insert(nodes.begin(), n, n + ncount);
        fread(r, sizeof(uint32_t), rcount, f);
        geom_refs.clear();
        geom_refs.insert(geom_refs.begin(), r, r + rcount);
        delete[] r;
        delete[] n;
        fclose(f);
    }

};

}}

#endif
