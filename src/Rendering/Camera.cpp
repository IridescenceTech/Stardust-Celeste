#include <Rendering/Camera.hpp>
#include <Rendering/RenderContext.hpp>
#include <gtc/matrix_transform.hpp>

namespace Stardust_Celeste::Rendering {

Camera::Camera(glm::vec3 pos, glm::vec3 rot, float fov, float aspect, float zN,
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
    glm::mat4 matrix(1.f);

    matrix = glm::rotate(matrix, rot.x, {1, 0, 0});
    matrix = glm::rotate(matrix, rot.y, {0, 1, 0});
    matrix = glm::rotate(matrix, rot.z, {0, 0, 1});

    matrix = glm::translate(matrix, pos);

    RenderContext::get().matrix_view(matrix);
}
} // namespace Stardust_Celeste::Rendering