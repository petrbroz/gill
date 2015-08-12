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
#include "core/intersection.h"

namespace gill { namespace core {

const int MaxTreeSegments = 64;
const int KdTreeFileMagic = 0xacc1;

/**
 * kD-tree for accelerating ray-to-geometry intersection tests.
 * @note Instead of generics, this implementation accepts two functions - one for computing
 * bounds of its enclosed geometries and one for testing the geometries for intersection with a ray.
 * This gives the clients a bit more flexibility as, for example, a triangle does not have to keep
 * a reference to its parent mesh (to get the actual vertices) - the mesh itself provides a lambda
 * function to the kD-tree that computes the bounds of a given triangle.
 */
class KdTree {

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
        KdTree::Node *node;
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
        uint32_t *overlapping, int num_overlapping, uint32_t *below, uint32_t *above, int depth);

public:
    KdTree(uint32_t geom_count, float isec_cost, float trav_cost, int max_geoms, int max_depth,
        std::function<BBox(uint32_t)> bounds_func,
        std::function<bool(uint32_t, const Ray&, float&, Intersection*)> isec_func);

    KdTree(const char *filename,
        std::function<BBox(uint32_t)> bounds_func,
        std::function<bool(uint32_t, const Ray&, float&, Intersection*)> isec_func);

    bool intersect(const Ray &ray, float &t, Intersection *isec);
    BBox bounds();
    void print_info();
    void print_dot();
    void save(const char *filename);
    void load(const char *filename);
};

}}

#endif
