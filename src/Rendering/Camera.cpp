#include <Rendering/Camera.hpp>
#include <Rendering/RenderContext.hpp>

namespace Stardust_Celeste::Rendering {

Camera::Camera(Math::Vector3<float> pos, Math::Vector3<float> rot, float fov, float aspect, float zN,
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
    Math::Matrix matrix(1.f);

    matrix *= Math::Matrix::Rotate(rot.x, {1, 0, 0});
    matrix *= Math::Matrix::Rotate(rot.y, {0, 1, 0});
    matrix *= Math::Matrix::Rotate(rot.z, {0, 0, 1});

    matrix *= Math::Matrix::Translate({-pos.x, -pos.y, -pos.z});

    RenderContext::get().matrix_perspective(fov, aspect, zNear, zFar);
    RenderContext::get().matrix_view(matrix);
}
} // namespace Stardust_Celeste::Rendering