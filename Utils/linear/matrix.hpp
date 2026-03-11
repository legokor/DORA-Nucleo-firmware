/*
 * matrix.hpp
 *
 *  Created on: Mar 5, 2025
 *      Author: dkiovics
 */

#ifndef LINEAR_MATRIX_HPP_
#define LINEAR_MATRIX_HPP_

#include "vector.hpp"

namespace lin {

class Matrix {
public:
    Vector col1, col2, col3;

    Matrix(Vector col1, Vector col2, Vector col3) : col1(col1), col2(col2), col3(col3) {}
    Matrix(float a) : col1(a), col2(a), col3(a) {}
    Matrix() : col1(1, 0, 0), col2(0, 1, 0), col3(0, 0, 1) {}

    Vector operator*(const Vector& v) const {
        return Vector(col1.x * v.x + col2.x * v.y + col3.x * v.z, col1.y * v.x + col2.y * v.y + col3.y * v.z,
                      col1.z * v.x + col2.z * v.y + col3.z * v.z);
    }
};

inline Vector operator*(const Vector& v, const Matrix& m) {
    return Vector(dot(v, m.col1), dot(v, m.col2), dot(v, m.col3));
}

} // namespace lin

#endif /* LINEAR_MATRIX_HPP_ */
