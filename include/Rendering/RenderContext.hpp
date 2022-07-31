/**
 * @file RenderContext.hpp
 * @author Nathan Bourgeois <iridescentrosesfall@gmail.com>
 * @brief Rendering Context
 * @version 0.1
 * @date 2021-12-26
 *
 * @copyright Copyright (c) 2021
 *
 */
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

struct RenderContextSettings {
    u32 width = 1280;
    u32 height = 720;
    const char *title = "Stardust App";
};

class RenderContext : public Singleton {

  private:
    bool is_init = false;
    Color c;

    glm::mat4 *_gfx_proj;
    glm::mat4 _gfx_persp, _gfx_ortho, _gfx_view, _gfx_model;
    std::vector<glm::mat4> _matrixStack;

  public:
    RenderContext()
        : _gfx_model(1), _gfx_ortho(1), _gfx_persp(1), _gfx_proj(&_gfx_ortho),
          _gfx_view(1), c{0xFF, 0xFF, 0xFF, 0xFF} {};

    auto initialize(const RenderContextSettings app) -> void;
    auto terminate() -> void;

    auto clear() -> void;
    inline auto set_color(Color color) -> void { c = color; }
    auto render() -> void;

    inline static auto get() -> RenderContext & {
        static RenderContext rctx;
        return rctx;
    }

    inline auto initialized() -> bool { return is_init; }

    auto set_matrices() -> void;

    auto matrix_push() -> void;
    auto matrix_pop() -> void;

    auto matrix_clear() -> void;

    auto matrix_translate(glm::vec3 v) -> void;
    auto matrix_rotate(glm::vec3 v) -> void;
    auto matrix_scale(glm::vec3 v) -> void;

    auto matrix_perspective(float fovy, float aspect, float zn, float zf)
        -> void;
    auto matrix_ortho(float l, float r, float b, float t, float zn, float zf)
        -> void;

    auto matrix_view(glm::mat4 mat) -> void;

    auto set_mode_2D() -> void;
    auto set_mode_3D() -> void;
};
} // namespace Stardust_Celeste::Rendering
