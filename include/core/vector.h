#ifndef GILL_CORE_VECTOR_H_
#define GILL_CORE_VECTOR_H_

#include <cmath>
#include <iostream>

namespace gill { namespace core {

struct Vector;
struct Point;
struct Normal;

/**
 * Common vector/point/normal operators and functions.
 * 
 * While it may not make sense to provide functions like 'normalize' for a Point class,
 * we still enable those for performance reasons. That way, for example, a normal
 * of a point on a sphere can be calculated without casting Point to Vector, normalizing,
 * and then casting Vector to Normal.
 */

template <typename V>
inline V operator-(const V &v) {
    return V(-v.x, -v.y, -v.z);
}

template <typename V>
inline bool operator==(const V &lhs, const V &rhs) {
    return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
}

template <typename V>
inline bool operator!=(const V &lhs, const V &rhs) {
    return lhs.x != rhs.x || lhs.y != rhs.y || lhs.z != rhs.z;
}

template <typename V>
inline V operator*(const V &v, float c) {
    return V(v.x * c, v.y * c, v.z * c);
}

template <typename V>
inline V& operator*=(V &v, float c) {
    v.x *= c; v.y *= c; v.z *= c; return v;
}

template <typename V>
inline V operator/(const V &v, float c) {
    return v * (1.0 / c);
}

template <typename V>
inline V& operator/=(V &v, float c) {
    v *= 1.0 / c; return v;
}

template <typename V>
inline float dot(const V &lhs, const V &rhs) {
    return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}

template <typename V>
inline float abs_dot(const V &lhs, const V &rhs) {
    return std::abs(lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z);
}

template <typename V>
inline V cross(const V &lhs, const V &rhs) {
    return V(lhs.y * rhs.z - lhs.z * rhs.y, lhs.z * rhs.x - lhs.x * rhs.z, lhs.x * rhs.y - lhs.y * rhs.x);
}

template <typename V>
inline float length_squared(const V &v) {
    return dot(v, v);
}

template <typename V>
inline float length(const V &v) {
    return std::sqrt(dot(v, v));
}

template <typename V>
inline V normalize(const V &v) {
    return v / length(v);
}

template <typename V>
inline float spherical_theta(const V &v) {
    return std::acos((v.z < -1.0) ? -1.0 : (v.z > 1.0) ? 1.0 : v.z);
}

template <typename V>
inline float spherical_phi(const V &v) {
    float p = std::atan2(v.y, v.x);
    return (p < 0.0) ? p + 2.0 * M_PI : p;
}

template <typename V>
inline bool has_nans(const V &v) {
    return std::isnan(v.x) || std::isnan(v.y) || std::isnan(v.z);
}

template <typename V>
inline V min(const V &v1, const V &v2) {
    return V(std::min(v1.x, v2.x), std::min(v1.y, v2.y), std::min(v1.z, v2.z));
}

template <typename V>
inline V max(const V &v1, const V &v2) {
    return V(std::max(v1.x, v2.x), std::max(v1.y, v2.y), std::max(v1.z, v2.z));
}

/**
 * 3D vector.
 */
struct Vector {
    float x, y, z;
    Vector(float value = 0.0) : x(value), y(value), z(value) {}
    Vector(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
    Vector(const Vector &v) : x(v.x), y(v.y), z(v.z) {}
    Vector(const Point &p);
    Vector(const Normal &n);
};

inline Vector operator+(const Vector &lhs, const Vector &rhs) {
    return Vector(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
}

inline Vector& operator+=(Vector &lhs, const Vector &rhs) {
    lhs.x += rhs.x; lhs.y += rhs.y; lhs.z += rhs.z; return lhs;
}

inline Vector operator-(const Vector &lhs, const Vector &rhs) {
    return Vector(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
}

inline Vector& operator-=(Vector &lhs, const Vector &rhs) {
    lhs.x -= rhs.x; lhs.y -= rhs.y; lhs.z -= rhs.z; return lhs;
}

inline Vector reflect(const Vector &v, const Vector &normal) {
    return -v + (normal * dot(normal, -v) + v) * 2.0;
}

std::ostream& operator<<(std::ostream &out, const Vector &v);

/**
 * 3D point.
 */
struct Point {
    float x, y, z;
    Point(float value = 0.0) : x(value), y(value), z(value) {}
    Point(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
    Point(const Point &p) : x(p.x), y(p.y), z(p.z) {}
    Point(const Vector &v);
    Point(const Normal &n);
};

inline Vector operator-(const Point &lhs, const Point &rhs) {
    return Vector(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
}

inline Point operator+(const Point &lhs, const Vector &rhs) {
    return Point(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
}

inline Point& operator+=(Point &lhs, const Vector &rhs) {
    lhs.x += rhs.x; lhs.y += rhs.y; lhs.z += rhs.z; return lhs;
}

inline Point operator-(const Point &lhs, const Vector &rhs) {
    return Point(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
}

inline Point& operator-=(Point &lhs, const Vector &rhs) {
    lhs.x -= rhs.x; lhs.y -= rhs.y; lhs.z -= rhs.z; return lhs;
}

inline float distance(const Point &p1, const Point &p2) {
    return length(p2 - p1);
}

std::ostream& operator<<(std::ostream &out, const Point &p);

/**
 * 3D normal.
 */
struct Normal {
    float x, y, z;
    Normal(float value = 0.0) : x(value), y(value), z(value) {}
    Normal(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
    Normal(const Normal &n) : x(n.x), y(n.y), z(n.z) {}
    Normal(const Vector &v);
    Normal(const Point &p);
};

std::ostream& operator<<(std::ostream &out, const Normal &n);

inline Vector::Vector(const Point &p) : x(p.x), y(p.y), z(p.z) {}
inline Vector::Vector(const Normal &n) : x(n.x), y(n.y), z(n.z) {}

inline Point::Point(const Vector &v) : x(v.x), y(v.y), z(v.z) {}
inline Point::Point(const Normal &n) : x(n.x), y(n.y), z(n.z) {}

inline Normal::Normal(const Vector &v) : x(v.x), y(v.y), z(v.z) {}
inline Normal::Normal(const Point &p) : x(p.x), y(p.y), z(p.z) {}

}}

#endif
