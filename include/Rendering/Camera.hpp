#pragma once
#include <glm.hpp>

namespace Stardust_Celeste::Rendering {

class Camera {
  public:
    Camera(glm::vec3 pos, glm::vec3 rot, float fov, float aspect, float zN,
           float zF);

    auto set_proj(float fov, float aspect, float zn, float zf) -> void;
    auto update() -> void;

    glm::vec3 pos;
    glm::vec3 rot;

  private:
    float fov, aspect, zNear, zFar;
};

} // namespace Stardust_Celeste::Rendering