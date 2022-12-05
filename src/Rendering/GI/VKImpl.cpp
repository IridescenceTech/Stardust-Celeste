#ifdef SDC_VULKAN
#include <Rendering/GI.hpp>
#include <optional>

#include <vulkan/vulkan.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>


#include "VK/VkContext.hpp"
#include "VK/VkPipeline.hpp"
#include "VK/VkBufferObject.hpp"
#include "VK/VkTextureHandle.hpp"

#include <set>
#include "Core/Application.hpp"
#include "Utilities/Assertion.hpp"

namespace GI {
    uint32_t programID;
    GLFWwindow* window;

    namespace detail {
        extern GLFWwindow* window;
    }

    std::vector<Vertex> vertices = {
            {0.0f, 0.0f, {{0xFF, 0x00, 0x00, 0xFF}}, -0.5f, -0.5f, 0.0f},
            {1.0f, 0.0f, {{0x00, 0xFF, 0x00, 0xFF}}, 0.5f, -0.5f, 0.0f},
            {1.0f, 1.0f, {{0x00, 0x00, 0xFF, 0xFF}}, 0.5f, 0.5f, 0.0f},
            {0.0f, 1.0f, {{0xFF, 0xFF, 0xFF, 0xFF}}, -0.5f, 0.5f, 0.0f},
    };

    std::vector<Vertex> verts2 = {
            {0.0f, 0.0f, {{0xFF, 0x00, 0x00, 0xFF}}, -0.5f, -0.5f, -0.5f},
            {1.0f, 0.0f, {{0x00, 0xFF, 0x00, 0xFF}}, 0.5f, -0.5f, -0.5f},
            {1.0f, 1.0f, {{0x00, 0x00, 0xFF, 0xFF}}, 0.5f, 0.5f, -0.5f},
            {0.0f, 1.0f, {{0xFF, 0xFF, 0xFF, 0xFF}}, -0.5f, 0.5f, -0.5f}
    };
    std::vector<uint16_t> indices = {
            0, 1, 2, 2, 3, 0,
    };

    detail::VKBufferObject* vbo, *vbo2;
    detail::VKTextureHandle* tex;
    detail::VKTextureHandle* grass;

    auto init(const RenderContextSettings app) -> void {
        detail::VKContext::get().init(app);
        window = detail::window;
        detail::VKPipeline::get().init();

        tex = detail::VKTextureHandle::create("resourcepacks/default/assets/minecraft/textures/dirt.png", VK_FILTER_NEAREST, VK_FILTER_NEAREST, true, false);
        grass = detail::VKTextureHandle::create("resourcepacks/default/assets/minecraft/textures/grass.png", VK_FILTER_NEAREST, VK_FILTER_NEAREST, true, false);
        vbo = detail::VKBufferObject::create(vertices, indices);
        vbo2 = detail::VKBufferObject::create(verts2, indices);
    }

    auto terminate() -> void {
        vbo->destroy();
        vbo2->destroy();
        tex->destroy();
        grass->destroy();

        detail::VKPipeline::get().deinit();
        detail::VKContext::get().deinit();
    }

    auto enable(u32 state) -> void {

    }
    auto disable(u32 state) -> void {

    }

    auto set_culling_mode(bool enabled, bool ccw) -> void {

    }

    auto depth_func(u32 mode) -> void {

    }
    auto blend_func(u32 src, u32 dest) -> void {

    }
    auto alpha_func(u32 func, u32 value, u32 mask) -> void {

    }

    auto start_frame(bool dialog) -> void {
        detail::VKPipeline::get().beginFrame();

        grass->bind();
        vbo->bind();
        vbo->draw();

        tex->bind();
        vbo2->bind();
        vbo2->draw();
    }
    auto end_frame(bool vsync, bool dialog) -> void {
        glfwPollEvents();

        detail::VKPipeline::get().endFrame();

        if (vsync)
            glfwSwapInterval(1);
        else
            glfwSwapInterval(0);

        if (glfwWindowShouldClose(window))
            Stardust_Celeste::Core::Application::get().exit();

        glfwSwapBuffers(window);
    }

    auto clear_color(Color color) -> void {

    }

    auto clear(u32 mask) -> void {

    }
}

#endif