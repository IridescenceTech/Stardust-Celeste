#include <Rendering/Camera.hpp>
#include <Rendering/RenderContext.hpp>

namespace Stardust_Celeste::Rendering {

Camera::Camera(mathfu::Vector<float, 3> pos, mathfu::Vector<float, 3> rot, float fov, float aspect, float zN,
               float zF) {
    this->pos = pos;
    this->rot = rot;

    set_proj(fov, aspect, zN, zF);
    update();
}

auto Camera::set_proj(float fov, float aspect, float zn, float zf) -> void {
    this->fov = fov;
    this->aspect = aspect;
    this->zNear = zn;
    this->zFar = zf;

    RenderContext::get().matrix_perspective(fov, aspect, zn, zf);
}

auto Camera::update() -> void {
    auto rot_x = mathfu::Matrix<float, 3, 3>::RotationX(rot.x / 180.0f * M_PI);
    auto rot_y = mathfu::Matrix<float, 3, 3>::RotationY(rot.y / 180.0f * M_PI);
    auto rot_z = mathfu::Matrix<float, 3, 3>::RotationZ(rot.z / 180.0f * M_PI);

    auto rotation = rot_x * rot_y * rot_z;
    auto rotation_matrix = mathfu::Matrix<float, 4, 4>::FromRotationMatrix(rotation);
    auto translation = mathfu::Matrix<float, 4, 4>::FromTranslationVector(pos);

    auto matrix = rotation_matrix * translation;

    RenderContext::get().matrix_perspective(fov, aspect, zNear, zFar);
    RenderContext::get().matrix_view(matrix);
}
} // namespace Stardust_Celeste::Rendering