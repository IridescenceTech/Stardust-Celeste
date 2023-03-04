#pragma once
#ifndef NO_EXPERIMENTAL_GRAPHICS

#include <optional>
#include <vector>
#include "Rendering/GI.hpp"

#include <vulkan/vulkan.h>

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