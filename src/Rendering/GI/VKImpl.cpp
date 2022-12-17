#ifdef SDC_VULKAN
#include <Rendering/GI.hpp>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "Rendering/GI/VK/VkBufferObject.hpp"
#include "Rendering/GI/VK/VkContext.hpp"
#include "Rendering/GI/VK/VkPipeline.hpp"
#include "Core/Application.hpp"
#include "Utilities/Assertion.hpp"

namespace GI {
    uint32_t programID;
    GLFWwindow* window;

    namespace detail {
        extern GLFWwindow* window;
    }

    auto init(const RenderContextSettings app) -> void {
        detail::VKContext::get().init(app);
        window = detail::window;
        detail::VKPipeline::get().init();
    }

    auto terminate() -> void {
        detail::VKPipeline::get().deinit();
        detail::VKContext::get().deinit();
    }

    auto enable(u32 state) -> void {
        if(state == GI_DEPTH_TEST) {
            vkCmdSetDepthTestEnable(detail::VKPipeline::get().commandBuffer, true);
        }
    }
    auto disable(u32 state) -> void {
        if(state == GI_DEPTH_TEST) {
            vkCmdSetDepthTestEnable(detail::VKPipeline::get().commandBuffer, false);
        }
    }

    auto set_culling_mode(bool enabled, bool ccw) -> void {
        VkCullModeFlags cullMode;
        if(enabled) {
            if(!ccw) {
                cullMode = VK_CULL_MODE_FRONT_BIT;
            } else {
                cullMode = VK_CULL_MODE_BACK_BIT;
            }
        } else {
            cullMode = VK_CULL_MODE_NONE;
        }

        vkCmdSetCullMode(detail::VKPipeline::get().commandBuffer, cullMode);
    }

    auto depth_func(u32 mode) -> void {
    }
    auto blend_func(u32 src, u32 dest) -> void {
        VkColorBlendEquationEXT blendEquationExt;
        blendEquationExt.srcColorBlendFactor = static_cast<VkBlendFactor>(src);
        blendEquationExt.dstColorBlendFactor = static_cast<VkBlendFactor>(dest);
        blendEquationExt.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        blendEquationExt.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        blendEquationExt.colorBlendOp = VK_BLEND_OP_ADD;
        blendEquationExt.alphaBlendOp = VK_BLEND_OP_ADD;

        auto fn = reinterpret_cast<PFN_vkCmdSetColorBlendEquationEXT>(vkGetDeviceProcAddr(detail::VKContext::get().logicalDevice, "vkCmdSetColorBlendEquationEXT"));
        fn(detail::VKPipeline::get().commandBuffer, 0, 1, &blendEquationExt);
    }
    auto alpha_func(u32 func, u32 value, u32 mask) -> void {

    }

    auto start_frame(bool dialog) -> void {
        detail::VKPipeline::get().beginFrame();
    }

    auto end_frame(bool vsync, bool dialog) -> void {
        static auto startTime = std::chrono::high_resolution_clock::now();

        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

        if(time > 1.0f / 60.0f){
            glfwPollEvents();
            startTime = currentTime;
        }


        detail::VKPipeline::get().endFrame();

        if (vsync)
            glfwSwapInterval(1);
        else
            glfwSwapInterval(0);

        if (glfwWindowShouldClose(window))
            Stardust_Celeste::Core::Application::get().exit();

        glfwSwapBuffers(window);
    }

    auto to_vec4(Color &c) -> glm::vec4 {
        return {static_cast<float>(c.rgba.r) / 255.0f,
                static_cast<float>(c.rgba.g) / 255.0f,
                static_cast<float>(c.rgba.b) / 255.0f,
                static_cast<float>(c.rgba.a) / 255.0f};
    }

    auto clear_color(Color color) -> void {
        detail::VKPipeline::get().clearColor = to_vec4(color);
    }

    auto clear(u32 mask) -> void {
        //This does nothing here -- it's part of the render pass
    }

    auto clearDepth() -> void {

    }
}

#endif