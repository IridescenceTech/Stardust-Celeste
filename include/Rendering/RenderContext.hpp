#pragma once

#include "Utilities/Assertion.hpp"
#include "Utilities/Logger.hpp"
#include "Utilities/Singleton.hpp"
#include "Utilities/Types.hpp"
#include <glm.hpp>
#include <vector>

#include "RenderTypes.hpp"

#include <Platform/Platform.hpp>
namespace Stardust_Celeste::Rendering {

/**
 * @brief Render Context Settings
 * width -- Width of window
 * height -- Height of window
 * title -- Title of window
 */
struct RenderContextSettings {
    u32 width = 1280;
    u32 height = 720;
    const char *title = "Stardust App";
};

class RenderContext final : public Singleton {

  private:
    bool is_init = false;
    Color c;

    glm::mat4 *_gfx_proj;
    glm::mat4 _gfx_persp, _gfx_ortho, _gfx_view, _gfx_model;
    std::vector<glm::mat4> _matrixStack;

  public:
    RenderContext()
        : c{0xFF, 0xFF, 0xFF, 0xFF}, _gfx_proj(&_gfx_ortho), _gfx_persp(1),
          _gfx_ortho(1), _gfx_view(1), _gfx_model(1){};

    /**
     * @brief Initialize a rendering context with settings
     *
     * @param app Settings to initialize with
     */
    auto initialize(const RenderContextSettings app) -> void;

    /**
     * @brief Terminate a rendering context
     *
     */
    auto terminate() -> void;

    /**
     * @brief Clear the screen
     *
     */
    auto clear() -> void;

    /**
     * @brief Set the color of the base screen
     *
     * @param color Color to use
     */
    inline auto set_color(Color color) -> void { c = color; }

    /**
     * @brief Render final to screen
     *
     */
    auto render() -> void;

    /**
     * @brief Get a static RenderContext
     *
     * @return RenderContext& Context
     */
    inline static auto get() -> RenderContext & {
        static RenderContext rctx;
        return rctx;
    }

    /**
     * @brief Is initialized?
     *
     * @return true
     * @return false
     */
    inline auto initialized() -> bool { return is_init; }

    /**
     * @brief Set the matrices for shaders
     *
     */
    auto set_matrices() -> void;

    /**
     * @brief Push the model matrix
     *
     */
    auto matrix_push() -> void;

    /**
     * @brief Pop back the model matrix
     *
     */
    auto matrix_pop() -> void;

    /**
     * @brief Clear model matrix
     *
     */
    auto matrix_clear() -> void;

    /**
     * @brief Translate model matrix by V
     *
     * @param v Translation
     */
    auto matrix_translate(glm::vec3 v) -> void;

    /**
     * @brief Rotate model matrix by V (angle X, angle Y, angle Z)
     *
     * @param v Rotation
     */
    auto matrix_rotate(glm::vec3 v) -> void;

    /**
     * @brief Scale mode matrix by V
     *
     * @param v Scale
     */
    auto matrix_scale(glm::vec3 v) -> void;

    /**
     * @brief Crerate perspective matrix
     *
     * @param fovy FOV
     * @param aspect Aspect Ratio
     * @param zn Z Near cutoff
     * @param zf Z Far cutoff
     */
    auto matrix_perspective(float fovy, float aspect, float zn, float zf)
        -> void;

    /**
     * @brief Create orthographic matrix
     *
     * @param l Left
     * @param r Right
     * @param b Bottom
     * @param t Top
     * @param zn ZNear
     * @param zf ZFar
     */
    auto matrix_ortho(float l, float r, float b, float t, float zn, float zf)
        -> void;

    /**
     * @brief Set View Matrix
     *
     * @param mat Matrix
     */
    auto matrix_view(glm::mat4 mat) -> void;

    /**
     * @brief Set the mode to 2D
     *
     */
    auto set_mode_2D() -> void;

    /**
     * @brief Set the mode to 3D
     *
     */
    auto set_mode_3D() -> void;
};
} // namespace Stardust_Celeste::Rendering
