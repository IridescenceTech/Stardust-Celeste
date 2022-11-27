#include <Core/Application.hpp>
#include <Platform/Platform.hpp>
#include <Rendering/GI.hpp>
#include <Rendering/RenderContext.hpp>
#define BUILD_PC (BUILD_PLAT == BUILD_WINDOWS || BUILD_PLAT == BUILD_POSIX)

#if BUILD_PC
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.hpp>
#include <string>
#elif BUILD_PLAT == BUILD_PSP

#define BUF_WIDTH (512)
#define SCR_WIDTH (480)
#define SCR_HEIGHT (272)

#include <intraFont.h>
#include <pspctrl.h>
#include <pspdebug.h>
#include <pspdisplay.h>
#include <pspge.h>
#include <pspgu.h>
#include <pspgum.h>
#include <pspkernel.h>
#include <psppower.h>
#include <psptypes.h>
#include <psputils.h>
#include <stdarg.h>

#endif
#include <Rendering/ShaderManager.hpp>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <gtx/string_cast.hpp>

namespace Stardust_Celeste::Rendering {

auto RenderContext::initialize(const RenderContextSettings app) -> void {
    GI::init(app);

    GI::start_frame();

    GI::enable(GI_DEPTH_TEST);
    GI::depth_func(GI_LEQUAL);

    GI::set_culling_mode(true, true);

    GI::enable(GI_BLEND);
    GI::blend_func(GI_SRC_ALPHA, GI_ONE_MINUS_SRC_ALPHA);

    GI::enable(GI_ALPHA_TEST);
    GI::alpha_func(GI_GREATER, 0x20, 0xFF);

    GI::enable(GI_TEXTURE_2D);

    GI::end_frame(true, false);

    // PSP: SETUP MATRICES
    c.color = 0xFFFFFFFF;
    is_init = true;
    _gfx_proj = &_gfx_ortho;
}

auto RenderContext::terminate() -> void { GI::terminate(); }

auto RenderContext::clear() -> void {
    GI::start_frame();

    GI::clear_color(c);
    GI::clear(GI_COLOR_BUFFER_BIT | GI_DEPTH_BUFFER_BIT |
              GI_STENCIL_BUFFER_BIT);
}

auto RenderContext::render() -> void { GI::end_frame(vsync); }

auto RenderContext::matrix_push() -> void {
    _matrixStack.push_back(_gfx_model);
    _gfx_model = glm::mat4(1.0f);
}

auto RenderContext::matrix_pop() -> void {
    _gfx_model = _matrixStack[_matrixStack.size() - 1];
    _matrixStack.pop_back();
}

auto RenderContext::matrix_clear() -> void {
    _gfx_model = glm::mat4(1.0f);
#if BUILD_PLAT == BUILD_PSP
    sceGumMatrixMode(GU_MODEL);
    sceGumLoadIdentity();
#endif
}

auto RenderContext::matrix_translate(glm::vec3 v) -> void {
    _gfx_model = glm::translate(_gfx_model, v);
}

auto RenderContext::matrix_rotate(glm::vec3 v) -> void {
    _gfx_model = glm::rotate(_gfx_model, v.x / 180.0f * 3.14159f, {1, 0, 0});
    _gfx_model = glm::rotate(_gfx_model, v.y / 180.0f * 3.14159f, {0, 1, 0});
    _gfx_model = glm::rotate(_gfx_model, v.z / 180.0f * 3.14159f, {0, 0, 1});
}

auto RenderContext::matrix_scale(glm::vec3 v) -> void {
    _gfx_model = glm::scale(_gfx_model, v);
}

auto RenderContext::matrix_perspective(float fovy, float aspect, float zn,
                                       float zf) -> void {
    _gfx_persp = glm::perspective(deg2rad(fovy), aspect, zn, zf);
#if BUILD_PC || BUILD_PLAT == BUILD_VITA
    _gfx_view = glm::mat4(1.0f);
    _gfx_model = glm::mat4(1.0f);
#elif BUILD_PLAT == BUILD_PSP
    sceGumMatrixMode(GU_PROJECTION);
    ScePspFMatrix4 m1 = *((ScePspFMatrix4 *)glm::value_ptr(*_gfx_proj));
    sceGumLoadMatrix(&m1);

    sceGumMatrixMode(GU_VIEW);
    sceGumLoadIdentity();
    sceGumMatrixMode(GU_MODEL);
    sceGumLoadIdentity();
#endif
}

auto RenderContext::matrix_ortho(float l, float r, float b, float t, float zn,
                                 float zf) -> void {
    _gfx_ortho = glm::ortho(l, r, b, t, zn, zf);
#if BUILD_PC || BUILD_PLAT == BUILD_VITA
    _gfx_view = glm::mat4(1.0f);
    _gfx_model = glm::mat4(1.0f);
#elif BUILD_PLAT == BUILD_PSP
    sceGumMatrixMode(GU_PROJECTION);
    ScePspFMatrix4 m1 = *((ScePspFMatrix4 *)glm::value_ptr(*_gfx_proj));
    sceGumLoadMatrix(&m1);

    sceGumMatrixMode(GU_VIEW);
    sceGumLoadIdentity();
    sceGumMatrixMode(GU_MODEL);
    sceGumLoadIdentity();
#endif
}

auto RenderContext::set_matrices() -> void {
#if BUILD_PC

    glm::mat4 newModel = glm::mat4(1.0f);
    for (int i = 0; i < _matrixStack.size(); i++) {
        newModel *= _matrixStack[i];
    }
    newModel *= _gfx_model;

    glBindBuffer(GL_UNIFORM_BUFFER,
                 ShaderManager::get().get_current_shader().ubo);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 2, sizeof(glm::mat4),
                    glm::value_ptr(newModel));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
#elif BUILD_PLAT == BUILD_VITA

    glm::mat4 newModel = glm::mat4(1.0f);
    for (int i = 0; i < _matrixStack.size(); i++) {
        newModel *= _matrixStack[i];
    }
    newModel *= _gfx_model;

    glUniformMatrix4fv(ShaderManager::get().get_current_shader().projLoc, 1,
                       GL_FALSE, glm::value_ptr(*_gfx_proj));
    glUniformMatrix4fv(ShaderManager::get().get_current_shader().viewLoc, 1,
                       GL_FALSE, glm::value_ptr(_gfx_view));
    glUniformMatrix4fv(ShaderManager::get().get_current_shader().modLoc, 1,
                       GL_FALSE, glm::value_ptr(newModel));

#elif BUILD_PLAT == BUILD_PSP

    glm::mat4 newModel = glm::mat4(1.0f);
    for (int i = 0; i < _matrixStack.size(); i++) {
        newModel *= _matrixStack[i];
    }
    newModel *= _gfx_model;

    sceGumMatrixMode(GU_MODEL);
    ScePspFMatrix4 m1 = *((ScePspFMatrix4 *)glm::value_ptr(newModel));
    sceGumLoadMatrix(&m1);
#endif
}

auto RenderContext::matrix_view(glm::mat4 mat) -> void {
#if BUILD_PC
    _gfx_view = mat;

    glBindBuffer(GL_UNIFORM_BUFFER,
                 ShaderManager::get().get_current_shader().ubo);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4),
                    glm::value_ptr(_gfx_view));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

#elif BUILD_PLAT == BUILD_PSP
    sceGumMatrixMode(GU_VIEW);
    ScePspFMatrix4 m1 = *((ScePspFMatrix4 *)glm::value_ptr(mat));
    sceGumLoadMatrix(&m1);
#elif BUILD_VITA
    _gfx_view = mat;
#endif
}

auto RenderContext::matrix_model(glm::mat4 mat) -> void {
#if BUILD_PC
    _gfx_model = mat;
#elif BUILD_PLAT == BUILD_PSP
    sceGumMatrixMode(GU_MODEL);
    ScePspFMatrix4 m1 = *((ScePspFMatrix4*)glm::value_ptr(mat));
    sceGumLoadMatrix(&m1);
#elif BUILD_VITA
    _gfx_model = mat;
#endif
}

auto RenderContext::set_mode_2D() -> void {
    _gfx_proj = &_gfx_ortho;
#if BUILD_PC
    glBindBuffer(GL_UNIFORM_BUFFER,
                 ShaderManager::get().get_current_shader().ubo);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4),
                    glm::value_ptr(*_gfx_proj));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    _gfx_view = glm::mat4(1.0f);

    glBindBuffer(GL_UNIFORM_BUFFER,
                 ShaderManager::get().get_current_shader().ubo);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4),
                    glm::value_ptr(_gfx_view));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    _gfx_model = glm::mat4(1.0f);
#elif BUILD_PLAT == BUILD_PSP
    sceGumMatrixMode(GU_PROJECTION);
    ScePspFMatrix4 m1 = *((ScePspFMatrix4 *)glm::value_ptr(*_gfx_proj));
    sceGumLoadMatrix(&m1);
    sceGumMatrixMode(GU_VIEW);
    sceGumLoadIdentity();
    sceGumMatrixMode(GU_MODEL);
    sceGumLoadIdentity();
#elif BUILD_PLAT == BUILD_VITA
    _gfx_view = glm::mat4(1.0f);
    _gfx_model = glm::mat4(1.0f);
#endif
}
auto RenderContext::set_mode_3D() -> void {
    _gfx_proj = &_gfx_persp;
#if BUILD_PC
    glBindBuffer(GL_UNIFORM_BUFFER,
                 ShaderManager::get().get_current_shader().ubo);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4),
                    glm::value_ptr(*_gfx_proj));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    _gfx_view = glm::mat4(1.0f);

    glBindBuffer(GL_UNIFORM_BUFFER,
                 ShaderManager::get().get_current_shader().ubo);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4),
                    glm::value_ptr(_gfx_view));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    _gfx_model = glm::mat4(1.0f);
#elif BUILD_PLAT == BUILD_PSP
    sceGumMatrixMode(GU_PROJECTION);
    ScePspFMatrix4 m1 = *((ScePspFMatrix4 *)glm::value_ptr(*_gfx_proj));
    sceGumLoadMatrix(&m1);
    sceGumMatrixMode(GU_VIEW);
    sceGumLoadIdentity();
    sceGumMatrixMode(GU_MODEL);
    sceGumLoadIdentity();
#elif BUILD_PLAT == BUILD_VITA
    _gfx_view = glm::mat4(1.0f);
    _gfx_model = glm::mat4(1.0f);
#endif
}

} // namespace Stardust_Celeste::Rendering