#pragma once
#include <cmath>
#include "Vector.hpp"
#include <string.h>
#include "MathUtils.hpp"

namespace Stardust_Celeste::Math {

    struct Matrix {
        union {
            float elements[16] {0};
            Vector4<float> rows[4];
        };

        Matrix() {
            memset(elements, 0, 4 * 4 * sizeof(float));
        }

        explicit Matrix(float diagonal) {
            memset(elements, 0, 4 * 4 * sizeof(float));
            elements[0 + 0 * 4] = diagonal;
            elements[1 + 1 * 4] = diagonal;
            elements[2 + 2 * 4] = diagonal;
            elements[3 + 3 * 4] = diagonal;
        }

        explicit Matrix(float* elements) {
            memcpy(this->elements, elements, 4 * 4 * sizeof(float));
        }

        explicit Matrix(const Vector4<float>& row0, const Vector4<float>& row1, const Vector4<float>& row2, const Vector4<float>& row3)
        {
            rows[0] = row0;
            rows[1] = row1;
            rows[2] = row2;
            rows[3] = row3;
        }

        static Matrix Identity() {
            return Matrix(1.0f);
        }

        Matrix& mul(const Matrix& other) {
            float data[16];
            for (size_t row = 0; row < 4; row++)
            {
                for (size_t col = 0; col < 4; col++)
                {
                    float sum = 0.0f;
                    for (size_t e = 0; e < 4; e++)
                    {
                        sum += elements[e + row * 4] * other.elements[col + e * 4];
                    }
                    data[col + row * 4] = sum;
                }
            }
            memcpy(elements, data, 4 * 4 * sizeof(float));
            return *this;
        }
        friend Matrix operator*(Matrix left, const Matrix& right) {
            return left.mul(right);
        }

        Matrix& operator*=(const Matrix& other) {
            return mul(other);
        }

        Vector3<float> mul(const Vector3<float>& other) const {
            return multMatrix(other, *this);
        }

        friend Vector3<float> operator*(const Matrix& left, const Vector3<float>& right) {
            return left.mul(right);
        }

        Vector4<float> mul(const Vector4<float>& other) const {
            return  multMatrix(other, *this);
        }

        friend Vector4<float> operator*(const Matrix& left, const Vector4<float>& right) {
            return left.mul(right);
        }

        Matrix& invert() {
            float temp[16];

            temp[0] = elements[5] * elements[10] * elements[15] -
                      elements[5] * elements[11] * elements[14] -
                      elements[9] * elements[6] * elements[15] +
                      elements[9] * elements[7] * elements[14] +
                      elements[13] * elements[6] * elements[11] -
                      elements[13] * elements[7] * elements[10];

            temp[4] = -elements[4] * elements[10] * elements[15] +
                      elements[4] * elements[11] * elements[14] +
                      elements[8] * elements[6] * elements[15] -
                      elements[8] * elements[7] * elements[14] -
                      elements[12] * elements[6] * elements[11] +
                      elements[12] * elements[7] * elements[10];

            temp[8] = elements[4] * elements[9] * elements[15] -
                      elements[4] * elements[11] * elements[13] -
                      elements[8] * elements[5] * elements[15] +
                      elements[8] * elements[7] * elements[13] +
                      elements[12] * elements[5] * elements[11] -
                      elements[12] * elements[7] * elements[9];

            temp[12] = -elements[4] * elements[9] * elements[14] +
                       elements[4] * elements[10] * elements[13] +
                       elements[8] * elements[5] * elements[14] -
                       elements[8] * elements[6] * elements[13] -
                       elements[12] * elements[5] * elements[10] +
                       elements[12] * elements[6] * elements[9];

            temp[1] = -elements[1] * elements[10] * elements[15] +
                      elements[1] * elements[11] * elements[14] +
                      elements[9] * elements[2] * elements[15] -
                      elements[9] * elements[3] * elements[14] -
                      elements[13] * elements[2] * elements[11] +
                      elements[13] * elements[3] * elements[10];

            temp[5] = elements[0] * elements[10] * elements[15] -
                      elements[0] * elements[11] * elements[14] -
                      elements[8] * elements[2] * elements[15] +
                      elements[8] * elements[3] * elements[14] +
                      elements[12] * elements[2] * elements[11] -
                      elements[12] * elements[3] * elements[10];

            temp[9] = -elements[0] * elements[9] * elements[15] +
                      elements[0] * elements[11] * elements[13] +
                      elements[8] * elements[1] * elements[15] -
                      elements[8] * elements[3] * elements[13] -
                      elements[12] * elements[1] * elements[11] +
                      elements[12] * elements[3] * elements[9];

            temp[13] = elements[0] * elements[9] * elements[14] -
                       elements[0] * elements[10] * elements[13] -
                       elements[8] * elements[1] * elements[14] +
                       elements[8] * elements[2] * elements[13] +
                       elements[12] * elements[1] * elements[10] -
                       elements[12] * elements[2] * elements[9];

            temp[2] = elements[1] * elements[6] * elements[15] -
                      elements[1] * elements[7] * elements[14] -
                      elements[5] * elements[2] * elements[15] +
                      elements[5] * elements[3] * elements[14] +
                      elements[13] * elements[2] * elements[7] -
                      elements[13] * elements[3] * elements[6];

            temp[6] = -elements[0] * elements[6] * elements[15] +
                      elements[0] * elements[7] * elements[14] +
                      elements[4] * elements[2] * elements[15] -
                      elements[4] * elements[3] * elements[14] -
                      elements[12] * elements[2] * elements[7] +
                      elements[12] * elements[3] * elements[6];

            temp[10] = elements[0] * elements[5] * elements[15] -
                       elements[0] * elements[7] * elements[13] -
                       elements[4] * elements[1] * elements[15] +
                       elements[4] * elements[3] * elements[13] +
                       elements[12] * elements[1] * elements[7] -
                       elements[12] * elements[3] * elements[5];

            temp[14] = -elements[0] * elements[5] * elements[14] +
                       elements[0] * elements[6] * elements[13] +
                       elements[4] * elements[1] * elements[14] -
                       elements[4] * elements[2] * elements[13] -
                       elements[12] * elements[1] * elements[6] +
                       elements[12] * elements[2] * elements[5];

            temp[3] = -elements[1] * elements[6] * elements[11] +
                      elements[1] * elements[7] * elements[10] +
                      elements[5] * elements[2] * elements[11] -
                      elements[5] * elements[3] * elements[10] -
                      elements[9] * elements[2] * elements[7] +
                      elements[9] * elements[3] * elements[6];

            temp[7] = elements[0] * elements[6] * elements[11] -
                      elements[0] * elements[7] * elements[10] -
                      elements[4] * elements[2] * elements[11] +
                      elements[4] * elements[3] * elements[10] +
                      elements[8] * elements[2] * elements[7] -
                      elements[8] * elements[3] * elements[6];

            temp[11] = -elements[0] * elements[5] * elements[11] +
                       elements[0] * elements[7] * elements[9] +
                       elements[4] * elements[1] * elements[11] -
                       elements[4] * elements[3] * elements[9] -
                       elements[8] * elements[1] * elements[7] +
                       elements[8] * elements[3] * elements[5];

            temp[15] = elements[0] * elements[5] * elements[10] -
                       elements[0] * elements[6] * elements[9] -
                       elements[4] * elements[1] * elements[10] +
                       elements[4] * elements[2] * elements[9] +
                       elements[8] * elements[1] * elements[6] -
                       elements[8] * elements[2] * elements[5];

            float determinant = elements[0] * temp[0] + elements[1] * temp[4] + elements[2] * temp[8] + elements[3] * temp[12];
            determinant = 1.0f / determinant;

            for (size_t i = 0; i < 4 * 4; i++)
                elements[i] = temp[i] * determinant;

            return *this;
        }

        static Matrix Orthographic(float left, float right, float bottom, float top, float near, float far);
        static Matrix Perspective(float fov, float aspectRatio, float near, float far);
        static Matrix LookAt(const Vector3<float>& camera, const Vector3<float>& object, const Vector3<float>& up);        static Matrix Translate(const Vector3<float>& translation);
        static Matrix Rotate(float angle, const Vector3<float>& axis);
        static Matrix Scale(const Vector3<float>& scale);
        static Matrix Invert(const Matrix& matrix);
        static Matrix Transpose(const Matrix& matrix);

    };
}