/*
 * vector.hpp
 *
 *  Created on: Mar 5, 2025
 *      Author: dkiovics
 */

#ifndef LINEAR_VECTOR_HPP_
#define LINEAR_VECTOR_HPP_

namespace lin {

struct Vector {
    float x, y, z;

    Vector(float x, float y, float z) : x(x), y(y), z(z) {}

    Vector(float a) : x(a), y(a), z(a) {}

    Vector() : x(0), y(0), z(0) {}

    Vector operator+(const Vector& v) const { return Vector(x + v.x, y + v.y, z + v.z); }

    Vector operator-(const Vector& v) const { return Vector(x - v.x, y - v.y, z - v.z); }

    Vector operator*(float a) const { return Vector(x * a, y * a, z * a); }

    Vector operator/(float a) const { return Vector(x / a, y / a, z / a); }

    Vector operator-() const { return Vector(-x, -y, -z); }

    Vector operator*(const Vector& v) const { return Vector(x * v.x, y * v.y, z * v.z); }

    Vector operator/(const Vector& v) const { return Vector(x / v.x, y / v.y, z / v.z); }
};

inline float dot(const Vector& a, const Vector& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

} // namespace lin

#endif /* LINEAR_VECTOR_HPP_ */
