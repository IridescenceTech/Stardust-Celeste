#pragma once
#include <mathfu/vector.h>

namespace Stardust_Celeste::Rendering {

/**
 * @brief Camera class which defines a camera in space and sets the rendering
 * constraints
 *
 */
class Camera {
  public:
    Camera(mathfu::Vector<float, 3> pos, mathfu::Vector<float, 3> rot, float fov, float aspect, float zN,
           float zF);
    virtual ~Camera() = default;

    /**
     * @brief Set the projection matrix
     *
     * @param fov FOV angle of camera
     * @param aspect Aspect Ratio (16:9)
     * @param zn Z Near boundary
     * @param zf Z Far boundary
     */
    virtual auto set_proj(float fov, float aspect, float zn, float zf) -> void;

    /**
     * @brief Update the matrix stack based on the camera position and rotation
     *
     */
    virtual auto update() -> void;

    /**
     * @brief Position of the Camera in 3D Space
     *
     */
    mathfu::Vector<float, 3> pos;

    /**
     * @brief Rotation of the Camera in 3D Space in degrees
     *
     */
    mathfu::Vector<float, 3> rot;

  protected:
    float fov, aspect, zNear, zFar;
};

} // namespace Stardust_Celeste::Rendering