#include <Math/Matrix.hpp>

namespace Stardust_Celeste::Math {

    Matrix Matrix::Orthographic(float left, float right, float bottom, float top, float near, float far) {
        Matrix result(1.0f);

        result.elements[0 + 0 * 4] = 2.0f / (right - left);

        result.elements[1 + 1 * 4] = 2.0f / (top - bottom);

        result.elements[2 + 2 * 4] = 2.0f / (near - far);

        result.elements[3 + 0 * 4] = (left + right) / (left - right);
        result.elements[3 + 1 * 4] = (bottom + top) / (bottom - top);
        result.elements[3 + 2 * 4] = (far + near) / (far - near);

        return result;
    }
    Matrix Matrix::Perspective(float fov, float aspectRatio, float near, float far) {
        Matrix result(1.0f);

        float q = 1.0f / tan(toRadians(0.5f * fov));
        float a = q / aspectRatio;

        float b = (near + far) / (near - far);
        float c = (2.0f * near * far) / (near - far);

        result.elements[0 + 0 * 4] = a;
        result.elements[1 + 1 * 4] = q;
        result.elements[2 + 2 * 4] = b;
        result.elements[2 + 3 * 4] = -1.0f;
        result.elements[3 + 2 * 4] = c;

        return result;
    }

    Matrix Matrix::LookAt(const Vector3<float>& camera, const Vector3<float>& object, const Vector3<float>& up) {

        Matrix result = Identity();
        Vector3<float> f = (object - camera).normalize();
        Vector3<float> s = f.cross(up.normalize());
        Vector3<float> u = s.cross(f);

        result.elements[0 + 0 * 4] = s.x;
        result.elements[0 + 1 * 4] = s.y;
        result.elements[0 + 2 * 4] = s.z;

        result.elements[1 + 0 * 4] = u.x;
        result.elements[1 + 1 * 4] = u.y;
        result.elements[1 + 2 * 4] = u.z;

        result.elements[2 + 0 * 4] = -f.x;
        result.elements[2 + 1 * 4] = -f.y;
        result.elements[2 + 2 * 4] = -f.z;

        return result * Translate(Vector3<float>(-camera.x, -camera.y, -camera.z));
    }

    Matrix Matrix::Translate(const Vector3<float>& translation) {
        Matrix result(1.0f);

        result.elements[3 + 0 * 4] = translation.x;
        result.elements[3 + 1 * 4] = translation.y;
        result.elements[3 + 2 * 4] = translation.z;

        return result;
    }

    Matrix Matrix::Rotate(float angle, const Vector3<float>& axis) {
        Matrix result(1.0f);

        float r = toRadians(angle);
        float c = cosf(r);
        float s = sinf(r);
        float omc = 1.0f - c;

        float x = axis.x;
        float y = axis.y;
        float z = axis.z;

        result.elements[0 + 0 * 4] = x * x * omc + c;
        result.elements[0 + 1 * 4] = y * x * omc + z * s;
        result.elements[0 + 2 * 4] = x * z * omc - y * s;

        result.elements[1 + 0 * 4] = x * y * omc - z * s;
        result.elements[1 + 1 * 4] = y * y * omc + c;
        result.elements[1 + 2 * 4] = y * z * omc + x * s;

        result.elements[2 + 0 * 4] = x * z * omc + y * s;
        result.elements[2 + 1 * 4] = y * z * omc - x * s;
        result.elements[2 + 2 * 4] = z * z * omc + c;

        return result;
    }

    Matrix Matrix::Scale(const Vector3<float>& scale) {
        Matrix result(1.0f);

        result.elements[0 + 0 * 4] = scale.x;
        result.elements[1 + 1 * 4] = scale.y;
        result.elements[2 + 2 * 4] = scale.z;

        return result;
    }

    Matrix Matrix::Invert(const Matrix& matrix) {
        Matrix result = matrix;
        return result.invert();
    }

    Matrix Matrix::Transpose(const Matrix& matrix) {
        return Matrix(
                Vector4<float>(matrix.rows[0].x, matrix.rows[1].x, matrix.rows[2].x, matrix.rows[3].x),
                Vector4<float>(matrix.rows[0].y, matrix.rows[1].y, matrix.rows[2].y, matrix.rows[3].y),
                Vector4<float>(matrix.rows[0].z, matrix.rows[1].z, matrix.rows[2].z, matrix.rows[3].z),
                Vector4<float>(matrix.rows[0].w, matrix.rows[1].w, matrix.rows[2].w, matrix.rows[3].w)
        );
    }
}