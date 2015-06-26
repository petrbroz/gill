#ifndef GILL_CORE_VECTOR_H_
#define GILL_CORE_VECTOR_H_

#include <cmath>
#include <iostream>
#include <type_traits>

namespace gill { namespace core {

inline bool almost_zero(float v) {
    return std::abs(v) < 0.0001;
}

struct Vector;
struct Point;
struct Normal;

/**
 * Common vector/point/normal operators and functions.
 *
 * While it is tempting to use templates to provide general operator overloads
 * and functions for Vector/Point/Normal, it is not straightforward to restrict
 * the templates to only allow the 3 types, and as a result of that the generic
 * templates may clash with other parts of the code, e.g., with the overloads
 * of std::vector<...>::iterator operators.
 * Therefore, the inline functions here will be intentionally copied for every
 * structure for which they make sense.
 */

struct Vector {
    float x, y, z;
    Vector(float value = 0.0) : x(value), y(value), z(value) {}
    Vector(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
    Vector(const Vector &v) : x(v.x), y(v.y), z(v.z) {}
    Vector(const Point &p);
    Vector(const Normal &n);
    
    inline float& operator[](int i) { return i == 0 ? x : (i == 1 ? y : z); }
    inline float operator[](int i) const { return i == 0 ? x : (i == 1 ? y : z); }
    inline bool operator==(const Vector &v) const { return x == v.x && y == v.y && z == v.z; }
    inline bool operator!=(const Vector &v) const { return x != v.x || y != v.y || z != v.z; }
    inline Vector operator-() const { return Vector(-x, -y, -z); }
    inline Vector operator+(const Vector &v) const { return Vector(x + v.x, y + v.y, z + v.z); }
    inline Vector& operator+=(const Vector &v) { x += v.x; y += v.y; z += v.z; return *this; }
    inline Vector operator-(const Vector &v) const { return Vector(x - v.x, y - v.y, z - v.z); }
    inline Vector& operator-=(const Vector &v) { x -= v.x; y -= v.y; z -= v.z; return *this; }
    inline Vector operator*(float c) const { return Vector(x * c, y * c, z * c); }
    inline Vector& operator*=(float c) { x *= c; y *= c; z *= c; return *this; }
    inline Vector operator/(float c) const { return *this * (1.0 / c); }
    inline Vector& operator/=(float c) { *this *= (1.0 / c); return *this; }
};

std::ostream& operator<<(std::ostream &out, const Vector &v);

struct Point {
    float x, y, z;
    Point(float value = 0.0) : x(value), y(value), z(value) {}
    Point(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
    Point(const Point &p) : x(p.x), y(p.y), z(p.z) {}
    Point(const Vector &v);
    Point(const Normal &n);

    inline float& operator[](int i) { return i == 0 ? x : (i == 1 ? y : z); }
    inline float operator[](int i) const { return i == 0 ? x : (i == 1 ? y : z); }
    inline bool operator==(const Point &p) const { return x == p.x && y == p.y && z == p.z; }
    inline bool operator!=(const Point &p) const { return x != p.x || y != p.y || z != p.z; }
    inline Point operator+(const Vector &v) const { return Point(x + v.x, y + v.y, z + v.z); }
    inline Point& operator+=(const Vector &v) { x += v.x; y += v.y; z += v.z; return *this; }
    inline Point operator-(const Vector &v) const { return Point(x - v.x, y - v.y, z - v.z); }
    inline Point& operator-=(const Vector &v) { x -= v.x; y -= v.y; z -= v.z; return *this; }
    inline Vector operator-(const Point &p) const { return Vector(x - p.x, y - p.y, z - p.z); }
    inline Point operator*(float c) const { return Point(x * c, y * c, z * c); }
    inline Point& operator*=(float c) { x *= c; y *= c; z *= c; return *this; }
    inline Point operator/(float c) const { return *this * (1.0 / c); }
    inline Point& operator/=(float c) { *this *= (1.0 / c); return *this; }
};

std::ostream& operator<<(std::ostream &out, const Point &p);

struct Normal {
    float x, y, z;
    Normal(float value = 0.0) : x(value), y(value), z(value) {}
    Normal(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
    Normal(const Normal &n) : x(n.x), y(n.y), z(n.z) {}
    Normal(const Vector &v);
    Normal(const Point &p);

    inline float& operator[](int i) { return i == 0 ? x : (i == 1 ? y : z); }
    inline float operator[](int i) const { return i == 0 ? x : (i == 1 ? y : z); }
    inline bool operator==(const Normal &n) const { return x == n.x && y == n.y && z == n.z; }
    inline bool operator!=(const Normal &n) const { return x != n.x || y != n.y || z != n.z; }
    inline Normal operator-() const { return Normal(-x, -y, -z); }
    inline Normal operator+(const Vector &v) const { return Normal(x + v.x, y + v.y, z + v.z); }
    inline Normal& operator+=(const Vector &v) { x += v.x; y += v.y; z += v.z; return *this; }
    inline Normal operator-(const Vector &v) const { return Normal(x - v.x, y - v.y, z - v.z); }
    inline Normal& operator-=(const Vector &v) { x -= v.x; y -= v.y; z -= v.z; return *this; }
    inline Normal operator*(float c) const { return Normal(x * c, y * c, z * c); }
    inline Normal& operator*=(float c) { x *= c; y *= c; z *= c; return *this; }
    inline Normal operator/(float c) const { return *this * (1.0 / c); }
    inline Normal& operator/=(float c) { *this *= (1.0 / c); return *this; }
};

std::ostream& operator<<(std::ostream &out, const Normal &n);

inline float dot(const Vector &lhs, const Vector &rhs) {
    return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}

inline float dot(const Normal &lhs, const Vector &rhs) {
    return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}

inline float abs_dot(const Vector &lhs, const Vector &rhs) {
    return std::abs(lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z);
}

inline Vector cross(const Vector &lhs, const Vector &rhs) {
    return Vector(lhs.y * rhs.z - lhs.z * rhs.y, lhs.z * rhs.x - lhs.x * rhs.z, lhs.x * rhs.y - lhs.y * rhs.x);
}

inline Vector reflect(const Vector &v, const Normal &n) {
    return -v + (n * dot(n, -v) + v) * 2.0;
}

inline float length_squared(const Vector &v) {
    return dot(v, v);
}

inline float length(const Vector &v) {
    return std::sqrt(dot(v, v));
}

inline Vector normalize(const Vector &v) {
    return v / length(v);
}

inline float spherical_theta(const Vector &v) {
    return std::acos((v.z < -1.0) ? -1.0 : (v.z > 1.0) ? 1.0 : v.z);
}

inline float spherical_phi(const Vector &v) {
    float p = std::atan2(v.y, v.x);
    return (p < 0.0) ? p + 2.0 * M_PI : p;
}

inline float distance(const Point &p1, const Point &p2) {
    return length(p2 - p1);
}

inline Vector::Vector(const Point &p) : x(p.x), y(p.y), z(p.z) {}
inline Vector::Vector(const Normal &n) : x(n.x), y(n.y), z(n.z) {}
inline Point::Point(const Vector &v) : x(v.x), y(v.y), z(v.z) {}
inline Point::Point(const Normal &n) : x(n.x), y(n.y), z(n.z) {}
inline Normal::Normal(const Vector &v) : x(v.x), y(v.y), z(v.z) {}
inline Normal::Normal(const Point &p) : x(p.x), y(p.y), z(p.z) {}

template <typename V>
inline bool has_nans(const V &v) {
    return std::isnan(v.x) || std::isnan(v.y) || std::isnan(v.z);
}

template <typename V>
inline V min(const V &v1, const V &v2) {
    return V(std::min(v1.x, v2.x), std::min(v1.y, v2.y), std::min(v1.z, v2.z));
}

template <typename V>
inline V min(const V &v1, const V &v2, const V &v3) {
    return V(std::min(v1.x, std::min(v2.x, v3.x)), std::min(v1.y, std::min(v2.y, v3.y)), std::min(v1.z, std::min(v2.z, v3.z)));
}

template <typename V>
inline V max(const V &v1, const V &v2) {
    return V(std::max(v1.x, v2.x), std::max(v1.y, v2.y), std::max(v1.z, v2.z));
}

template <typename V>
inline V max(const V &v1, const V &v2, const V &v3) {
    return V(std::max(v1.x, std::max(v2.x, v3.x)), std::max(v1.y, std::max(v2.y, v3.y)), std::max(v1.z, std::max(v2.z, v3.z)));
}

}}

#endif
