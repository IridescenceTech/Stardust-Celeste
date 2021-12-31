/**
 * @file Camera.hpp
 * @author Nathan Bourgeois <iridescentrosesfall@gmail.com>
 * @brief
 * @version 0.1
 * @date 2021-12-29
 *
 * @copyright Copyright (c) 2021
 *
 */
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