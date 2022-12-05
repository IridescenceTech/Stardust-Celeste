#pragma once

#ifdef SDC_VULKAN
#include <Rendering/GI.hpp>
#include <optional>

#include <vulkan/vulkan.h>

#include "Core/Application.hpp"
#include "Utilities/Assertion.hpp"

namespace GI::detail {
    class VKContext : public Singleton {
    public:
        inline static auto get() -> VKContext& {
            static VKContext vkc;
            return vkc;
        }

        void init(const RenderContextSettings app);
        void deinit();

        VkInstance instance;
        VkSurfaceKHR surface;
        VkPhysicalDevice physicalDevice;
        VkDevice logicalDevice;
        VkQueue graphicsQueue, presentQueue;

        VkSwapchainKHR swapChain;
        std::vector<VkImage> swapChainImages;
        VkFormat swapChainImageFormat;
        VkExtent2D swapChainExtent;
        std::vector<VkImageView> swapChainImageViews;
    private:
        VKContext() = default;

    };
}

#endif