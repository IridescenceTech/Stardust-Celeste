#pragma once
#include <cmath>

namespace Stardust_Celeste::Math {

    struct Matrix;

    template <typename T>
    struct Vector2 {
        T x, y;

        Vector2() {
            x = 0;
            y = 0;
        }

        Vector2(float scalar) {
            x = y = scalar;
        }

        Vector2(float nx, float ny) {
            x = nx;
            y = ny;
        }

        Vector2& add(const Vector2& other) {
            x += other.x;
            y += other.y;
            return *this;
        }

        Vector2& sub(const Vector2& other) {
            x -= other.x;
            y -= other.y;
            return *this;
        }

        Vector2& mul(const Vector2& other) {
            x *= other.x;
            y *= other.y;
            return *this;
        }

        Vector2& div(const Vector2& other) {
            x /= other.x;
            y /= other.y;
            return *this;
        }

        Vector2& add(float value) {
            x += value;
            y += value;
            return *this;
        }

        Vector2& sub(float value) {
            x -= value;
            y -= value;
            return *this;
        }

        Vector2& mul(float value) {
            x *= value;
            y *= value;
            return *this;
        }

        Vector2& div(float value) {
            x /= value;
            y /= value;
            return *this;
        }

        friend Vector2 operator+(Vector2 left, const Vector2& right)
        {
            return left.add(right);
        }

        friend Vector2 operator-(Vector2 left, const Vector2& right)
        {
            return left.sub(right);
        }

        friend Vector2 operator*(Vector2 left, const Vector2& right)
        {
            return left.mul(right);
        }

        friend Vector2 operator/(Vector2 left, const Vector2& right)
        {
            return left.div(right);
        }

        friend Vector2 operator+(Vector2 left, float value)
        {
            return Vector2(left.x + value, left.y + value);
        }

        friend Vector2 operator-(Vector2 left, float value)
        {
            return Vector2(left.x - value, left.y - value);
        }

        friend Vector2 operator*(Vector2 left, float value)
        {
            return Vector2(left.x * value, left.y * value);
        }

        friend Vector2 operator/(Vector2 left, float value)
        {
            return Vector2(left.x / value, left.y / value);
        }

        Vector2& operator+=(const Vector2& other)
        {
            return add(other);
        }

        Vector2& operator-=(const Vector2& other)
        {
            return sub(other);
        }

        Vector2& operator*=(const Vector2& other)
        {
            return mul(other);
        }

        Vector2& operator/=(const Vector2& other)
        {
            return div(other);
        }

        Vector2& operator+=(float value)
        {
            return add(value);
        }

        Vector2& operator-=(float value)
        {
            return sub(value);
        }

        Vector2& operator*=(float value)
        {
            return mul(value);
        }

        Vector2& operator/=(float value)
        {
            return div(value);
        }

        bool operator==(const Vector2& other) const
        {
            return x == other.x && y == other.y;
        }

        bool operator!=(const Vector2& other) const
        {
            return !(*this == other);
        }

        bool operator<(const Vector2& other) const
        {
            return x < other.x && y < other.y;
        }

        bool operator<=(const Vector2& other) const
        {
            return x <= other.x && y <= other.y;
        }

        bool operator>(const Vector2& other) const
        {
            return x > other.x && y > other.y;
        }

        bool operator>=(const Vector2& other) const
        {
            return x >= other.x && y >= other.y;
        }

        float distance(const Vector2& other) const
        {
            float a = x - other.x;
            float b = y - other.y;
            return sqrt(a * a + b * b);
        }

        float dot(const Vector2& other) const
        {
            return x * other.x + y * other.y;
        }

        float magnitude() const
        {
            return sqrt(x * x + y * y);
        }

        Vector2 normalize() const
        {
            float length = magnitude();
            return Vector2(x / length, y / length);
        }
    };

    template <typename T>
    struct Vector3 {
        T x, y, z;

        Vector3() {
            x = 0;
            y = 0;
            z = 0;
        }

        Vector3(float scalar) {
            x = y = z = scalar;
        }

        Vector3(float nx, float ny, float nz) {
            x = nx;
            y = ny;
            z = nz;
        }

        Vector3& add(const Vector3& other) {
            x += other.x;
            y += other.y;
            z += other.z;
            return *this;
        }

        Vector3& sub(const Vector3& other) {
            x -= other.x;
            y -= other.y;
            z -= other.z;
            return *this;
        }

        Vector3& mul(const Vector3& other) {
            x *= other.x;
            y *= other.y;
            z *= other.z;
            return *this;
        }

        Vector3& div(const Vector3& other) {
            x /= other.x;
            y /= other.y;
            z /= other.z;
            return *this;
        }

        Vector3& add(float value) {
            x += value;
            y += value;
            z += value;
            return *this;
        }

        Vector3& sub(float value) {
            x -= value;
            y -= value;
            z -= value;
            return *this;
        }

        Vector3& mul(float value) {
            x *= value;
            y *= value;
            z *= value;
            return *this;
        }

        Vector3& div(float value) {
            x /= value;
            y /= value;
            z /= value;
            return *this;
        }

        friend Vector3 operator+(Vector3 left, const Vector3& right)
        {
            return left.add(right);
        }

        friend Vector3 operator-(Vector3 left, const Vector3& right)
        {
            return left.sub(right);
        }

        friend Vector3 operator*(Vector3 left, const Vector3& right)
        {
            return left.mul(right);
        }

        friend Vector3 operator/(Vector3 left, const Vector3& right)
        {
            return left.div(right);
        }

        friend Vector3 operator+(Vector3 left, float value)
        {
            return Vector3(left.x + value, left.y + value, left.z + value);
        }

        friend Vector3 operator-(Vector3 left, float value)
        {
            return Vector3(left.x - value, left.y - value, left.z - value);
        }

        friend Vector3 operator*(Vector3 left, float value)
        {
            return Vector3(left.x * value, left.y * value, left.z * value);
        }

        friend Vector3 operator/(Vector3 left, float value)
        {
            return Vector3(left.x / value, left.y / value, left.z / value);
        }

        Vector3& operator+=(const Vector3& other)
        {
            return add(other);
        }

        Vector3& operator-=(const Vector3& other)
        {
            return sub(other);
        }

        Vector3& operator*=(const Vector3& other)
        {
            return mul(other);
        }

        Vector3& operator/=(const Vector3& other)
        {
            return div(other);
        }

        Vector3& operator+=(float value)
        {
            return add(value);
        }

        Vector3& operator-=(float value)
        {
            return sub(value);
        }

        Vector3& operator*=(float value)
        {
            return mul(value);
        }

        Vector3& operator/=(float value)
        {
            return div(value);
        }

        bool operator==(const Vector3& other) const
        {
            return x == other.x && y == other.y;
        }

        bool operator!=(const Vector3& other) const
        {
            return !(*this == other);
        }

        bool operator<(const Vector3& other) const
        {
            return x < other.x && y < other.y && z < other.z;
        }

        bool operator<=(const Vector3& other) const
        {
            return x <= other.x && y <= other.y && z <= other.z;
        }

        bool operator>(const Vector3& other) const
        {
            return x > other.x && y > other.y && z > other.z;
        }

        bool operator>=(const Vector3& other) const
        {
            return x >= other.x && y >= other.y && z >= other.z;
        }

        float distance(const Vector3& other) const
        {
            float a = x - other.x;
            float b = y - other.y;
            float c = z - other.z;
            return sqrt(a * a + b * b + c * c);
        }

        float dot(const Vector3& other) const
        {
            return x * other.x + y * other.y + z * other.z;
        }

        float magnitude() const
        {
            return sqrt(x * x + y * y + z * z);
        }

        Vector3 normalize() const
        {
            float length = magnitude();
            return Vector3(x / length, y / length, z / length);
        }

        Vector3 cross(const Vector3& other) const
        {
            return Vector3(y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x);
        }

    };


    template <typename T>
    struct Vector4 {
        T x, y, z, w;

        Vector4() {
            x = 0;
            y = 0;
            z = 0;
            w = 0;
        }

        Vector4(float scalar) {
            x = y = z = w = scalar;
        }

        Vector4(float nx, float ny, float nz, float nw) {
            x = nx;
            y = ny;
            z = nz;
            w = nw;
        }

        Vector4& add(const Vector4 & other) {
            x += other.x;
            y += other.y;
            z += other.z;
            w += other.w;
            return *this;
        }

        Vector4& sub(const Vector4 & other) {
            x -= other.x;
            y -= other.y;
            z -= other.z;
            w -= other.w;
            return *this;
        }

        Vector4 & mul(const Vector4 & other) {
            x *= other.x;
            y *= other.y;
            z *= other.z;
            w *= other.w;
            return *this;
        }

        Vector4 & div(const Vector4 & other) {
            x /= other.x;
            y /= other.y;
            z /= other.z;
            w /= other.w;
            return *this;
        }

        Vector4 & add(float value) {
            x += value;
            y += value;
            z += value;
            w += value;
            return *this;
        }

        Vector4 & sub(float value) {
            x -= value;
            y -= value;
            z -= value;
            w -= value;
            return *this;
        }

        Vector4 & mul(float value) {
            x *= value;
            y *= value;
            z *= value;
            w *= value;
            return *this;
        }

        Vector4 & div(float value) {
            x /= value;
            y /= value;
            z /= value;
            w /= value;
            return *this;
        }

        friend Vector4 operator+(Vector4 left, const Vector4 & right)
        {
            return left.add(right);
        }

        friend Vector4 operator-(Vector4 left, const Vector4& right)
        {
            return left.sub(right);
        }

        friend Vector4 operator*(Vector4 left, const Vector4& right)
        {
            return left.mul(right);
        }

        friend Vector4 operator/(Vector4 left, const Vector4& right)
        {
            return left.div(right);
        }

        friend Vector4 operator+(Vector4 left, float value)
        {
            return Vector4(left.x + value, left.y + value, left.z + value, left.w + value);
        }

        friend Vector4 operator-(Vector4 left, float value)
        {
            return Vector4(left.x - value, left.y - value, left.z - value, left.w - value);
        }

        friend Vector4 operator*(Vector4 left, float value)
        {
            return Vector4(left.x * value, left.y * value, left.z * value, left.w * value);
        }

        friend Vector4 operator/(Vector4 left, float value)
        {
            return Vector4(left.x / value, left.y / value, left.z / value, left.w / value);
        }

        Vector4& operator+=(const Vector4& other)
        {
            return add(other);
        }

        Vector4& operator-=(const Vector4& other)
        {
            return sub(other);
        }

        Vector4& operator*=(const Vector4& other)
        {
            return mul(other);
        }

        Vector4& operator/=(const Vector4& other)
        {
            return div(other);
        }

        Vector4& operator+=(float value)
        {
            return add(value);
        }

        Vector4& operator-=(float value)
        {
            return sub(value);
        }

        Vector4& operator*=(float value)
        {
            return mul(value);
        }

        Vector4& operator/=(float value)
        {
            return div(value);
        }

        bool operator==(const Vector4& other) const
        {
            return x == other.x && y == other.y && z == other.z;
        }

        bool operator!=(const Vector4& other) const
        {
            return !(*this == other);
        }

        bool operator<(const Vector4& other) const
        {
            return x < other.x && y < other.y && z < other.z && w < other.w;
        }

        bool operator<=(const Vector4& other) const
        {
            return x <= other.x && y <= other.y && z <= other.z && w <= other.w;
        }

        bool operator>(const Vector4& other) const
        {
            return x > other.x && y > other.y && z > other.z && w > other.w;
        }

        bool operator>=(const Vector4& other) const
        {
            return x >= other.x && y >= other.y && z >= other.z && w >= other.w;
        }

        float distance(const Vector4& other) const
        {
            float a = x - other.x;
            float b = y - other.y;
            float c = z - other.z;
            float d = w - other.w;
            return sqrt(a * a + b * b + c * c + d * d);
        }

        float dot(const Vector4& other) const
        {
            return x * other.x + y * other.y + z * other.z + w * other.w;
        }

        float magnitude() const
        {
            return sqrt(x * x + y * y + z * z + w * w);
        }

        Vector4 normalize() const
        {
            float length = magnitude();
            return Vector4(x / length, y / length, z / length, w / length);
        }
    };

    Vector3<float> multMatrix(Vector3<float> v, Matrix m);
    Vector4<float> multMatrix(Vector4<float> v, Matrix m);

}