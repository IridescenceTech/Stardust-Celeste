#include <Core/Application.hpp>
#include <Platform/Platform.hpp>
#include <Rendering/GI.hpp>
#include <Rendering/RenderContext.hpp>
#define BUILD_PC (BUILD_PLAT == BUILD_WINDOWS || BUILD_PLAT == BUILD_POSIX)

#if BUILD_PLAT == BUILD_3DS
#include <GL/glu.h>
#endif

#if BUILD_PC
#define GLFW_INCLUDE_NONE
#include <glad/glad.hpp>
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

#include <Rendering/GI/VK/VkPipeline.hpp>

#if BUILD_PC
namespace GI {
    extern GLuint programID;
    extern GLuint ubo;
    extern u32 projLoc, viewLoc, modLoc;
}
#endif

namespace Stardust_Celeste::Rendering {

    RenderContextSettings rctxSettings;

auto RenderContext::initialize(const RenderContextSettings app) -> void {
    rctxSettings = app;
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

    vsync = false;
    GI::end_frame(vsync, false);

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
    _gfx_model = Math::Matrix::Identity();
}

auto RenderContext::matrix_pop() -> void {
    _gfx_model = _matrixStack[_matrixStack.size() - 1];
    _matrixStack.pop_back();
}

auto RenderContext::matrix_clear() -> void {
    _gfx_model = Math::Matrix::Identity();
#if BUILD_PLAT == BUILD_PSP
    sceGumMatrixMode(GU_MODEL);
    sceGumLoadIdentity();
#elif BUILD_PLAT == BUILD_3DS
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
#endif
}

auto RenderContext::matrix_translate(Math::Vector3<float> v) -> void {
    _gfx_model *= Math::Matrix::Translate(v);
}

auto RenderContext::matrix_rotate(Math::Vector3<float> v) -> void {
    _gfx_model *= Math::Matrix::Rotate(Math::toRadians(v.x), {1, 0, 0});
    _gfx_model *= Math::Matrix::Rotate(Math::toRadians(v.y), {0, 1, 0});
    _gfx_model *= Math::Matrix::Rotate(Math::toRadians(v.z), {0, 0, 1});
}

auto RenderContext::matrix_scale(Math::Vector3<float> v) -> void {
    _gfx_model *= Math::Matrix::Scale(v);
}

auto RenderContext::matrix_perspective(float fovy, float aspect, float zn,
                                       float zf) -> void {
    _gfx_persp = Math::Matrix::Perspective(Math::toRadians(fovy), aspect, zn, zf);
    if(rctxSettings.renderingApi == Vulkan) {
        _gfx_persp.rows[1].y *= -1;
    }
    _gfx_view = Math::Matrix::Identity();
    _gfx_model = Math::Matrix::Identity();
#if BUILD_PLAT == BUILD_PSP
    sceGumMatrixMode(GU_PROJECTION);
    ScePspFMatrix4 m1 = *((ScePspFMatrix4 *)glm::value_ptr(*_gfx_proj));
    sceGumLoadMatrix(&m1);

    sceGumMatrixMode(GU_VIEW);
    sceGumLoadIdentity();
    sceGumMatrixMode(GU_MODEL);
    sceGumLoadIdentity();
#elif BUILD_PLAT == BUILD_3DS
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(glm::value_ptr(*_gfx_proj));

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
#endif
}

auto RenderContext::matrix_ortho(float l, float r, float b, float t, float zn,
                                 float zf) -> void {
    if(rctxSettings.renderingApi == Vulkan) {
        _gfx_ortho = Math::Matrix::Orthographic(l, r, b, t, zn, zf);
    } else {
        _gfx_ortho = Math::Matrix::Orthographic(l, r, b, t, zn, zf);
    }

    _gfx_view = Math::Matrix::Identity();
    _gfx_model = Math::Matrix::Identity();
#if BUILD_PLAT == BUILD_PSP
    sceGumMatrixMode(GU_PROJECTION);
    ScePspFMatrix4 m1 = *((ScePspFMatrix4 *)glm::value_ptr(*_gfx_proj));
    sceGumLoadMatrix(&m1);

    sceGumMatrixMode(GU_VIEW);
    sceGumLoadIdentity();
    sceGumMatrixMode(GU_MODEL);
    sceGumLoadIdentity();
#elif BUILD_PLAT == BUILD_3DS
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(glm::value_ptr(*_gfx_proj));

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
#endif
}

auto RenderContext::set_matrices() -> void {
#if BUILD_PC
    Math::Matrix newModel = Math::Matrix::Identity();
    for (int i = 0; i < _matrixStack.size(); i++) {
        newModel *= _matrixStack[i];
    }
    newModel *= _gfx_model;

    if(rctxSettings.renderingApi == Vulkan) {
#ifndef NO_EXPERIMENTAL_GRAPHICS
        GI::detail::VKPipeline::get().ubo.model = newModel;
#endif
    } else {
        glBindBuffer(GL_UNIFORM_BUFFER, GI::ubo);
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(Math::Matrix) * 2, sizeof(Math::Matrix),
                        newModel.elements);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }
#elif BUILD_PLAT == BUILD_VITA

    glm::mat4 newModel = glm::mat4(1.0f);
    for (int i = 0; i < _matrixStack.size(); i++) {
        newModel *= _matrixStack[i];
    }
    newModel *= _gfx_model;

    glUniformMatrix4fv(GI::projLoc, 1,
                       GL_FALSE, glm::value_ptr(*_gfx_proj));
    glUniformMatrix4fv(GI::viewLoc, 1,
                       GL_FALSE, glm::value_ptr(_gfx_view));
    glUniformMatrix4fv(GI::modLoc, 1,
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

#elif BUILD_PLAT == BUILD_3DS
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(glm::value_ptr(*_gfx_proj));

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glLoadMatrixf(glm::value_ptr(_gfx_view));
    for (int i = 0; i < _matrixStack.size(); i++) {
        glMultMatrixf(glm::value_ptr(_matrixStack[i]));
    }
    glMultMatrixf(glm::value_ptr(_gfx_model));
#endif
}

auto RenderContext::matrix_view(Math::Matrix mat) -> void {
#if BUILD_PC
    _gfx_view = mat;

    if(rctxSettings.renderingApi == Vulkan) {
#ifndef NO_EXPERIMENTAL_GRAPHICS
        GI::detail::VKPipeline::get().ubo.projview = *_gfx_proj * _gfx_view;
#endif
    } else {
        glBindBuffer(GL_UNIFORM_BUFFER, GI::ubo);
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(Math::Matrix), sizeof(Math::Matrix),
                        _gfx_view.elements);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }
#elif BUILD_PLAT == BUILD_PSP
    sceGumMatrixMode(GU_VIEW);
    ScePspFMatrix4 m1 = *((ScePspFMatrix4 *)glm::value_ptr(mat));
    sceGumLoadMatrix(&m1);
#elif BUILD_PLAT == BUILD_VITA || BUILD_PLAT == BUILD_3DS
    _gfx_view = mat;
#endif
}

auto RenderContext::matrix_model(Math::Matrix mat) -> void {
#if BUILD_PC || BUILD_PLAT == BUILD_VITA || BUILD_PLAT == BUILD_3DS
    _gfx_model = mat;
#elif BUILD_PLAT == BUILD_PSP
    sceGumMatrixMode(GU_MODEL);
    ScePspFMatrix4 m1 = *((ScePspFMatrix4 *)glm::value_ptr(mat));
    sceGumLoadMatrix(&m1);
#endif
}

auto RenderContext::set_mode_2D() -> void {
    _gfx_proj = &_gfx_ortho;
#if BUILD_PC

    if(rctxSettings.renderingApi == Vulkan) {
        _gfx_view = Math::Matrix::Identity();
        _gfx_model = Math::Matrix::Identity();
#ifndef NO_EXPERIMENTAL_GRAPHICS

        GI::detail::VKPipeline::get().ubo.projview = *_gfx_proj * _gfx_view;
        GI::detail::VKPipeline::get().ubo.model = _gfx_model;
#endif
    } else {
        _gfx_view = Math::Matrix::Identity();
        _gfx_model = Math::Matrix::Identity();

        glBindBuffer(GL_UNIFORM_BUFFER, GI::ubo);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Math::Matrix),
                        (*_gfx_proj).elements);
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(Math::Matrix), sizeof(Math::Matrix),
                        _gfx_view.elements);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }
#elif BUILD_PLAT == BUILD_PSP
    sceGumMatrixMode(GU_PROJECTION);
    ScePspFMatrix4 m1 = *((ScePspFMatrix4 *)glm::value_ptr(*_gfx_proj));
    sceGumLoadMatrix(&m1);
    sceGumMatrixMode(GU_VIEW);
    sceGumLoadIdentity();
    sceGumMatrixMode(GU_MODEL);
    sceGumLoadIdentity();
#elif BUILD_PLAT == BUILD_VITA || BUILD_PLAT == BUILD_3DS
    _gfx_view = glm::mat4(1.0f);
    _gfx_model = glm::mat4(1.0f);
#endif
}
auto RenderContext::set_mode_3D() -> void {
    _gfx_proj = &_gfx_persp;
#if BUILD_PC
    _gfx_view = Math::Matrix::Identity();
    _gfx_model = Math::Matrix::Identity();

    if(rctxSettings.renderingApi == Vulkan) {
#ifndef NO_EXPERIMENTAL_GRAPHICS

        GI::detail::VKPipeline::get().ubo.projview = *_gfx_proj * _gfx_view;
        GI::detail::VKPipeline::get().ubo.model = _gfx_model;
#endif
    } else {
        glBindBuffer(GL_UNIFORM_BUFFER, GI::ubo);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Math::Matrix),
                        (*_gfx_proj).elements);
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(Math::Matrix), sizeof(Math::Matrix),
                        _gfx_view.elements);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }
#elif BUILD_PLAT == BUILD_PSP
    sceGumMatrixMode(GU_PROJECTION);
    ScePspFMatrix4 m1 = *((ScePspFMatrix4 *)glm::value_ptr(*_gfx_proj));
    sceGumLoadMatrix(&m1);
    sceGumMatrixMode(GU_VIEW);
    sceGumLoadIdentity();
    sceGumMatrixMode(GU_MODEL);
    sceGumLoadIdentity();
#elif BUILD_PLAT == BUILD_VITA || BUILD_PLAT == BUILD_3DS
    _gfx_view = glm::mat4(1.0f);
    _gfx_model = glm::mat4(1.0f);
#endif
}

} // namespace Stardust_Celeste::Rendering