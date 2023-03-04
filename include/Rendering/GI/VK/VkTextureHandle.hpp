#pragma once

#ifndef NO_EXPERIMENTAL_GRAPHICS
#include <vulkan/vulkan.h>
#include <string>
#include <Rendering/GI/TextureHandle.hpp>
#include "Rendering/RenderTypes.hpp"
#include "stb_image.hpp"

namespace GI::detail{
    class VKTextureHandle final : public TextureHandle {
    public:
        VKTextureHandle() : textureImage(VK_NULL_HANDLE), textureImageMemory(VK_NULL_HANDLE), textureImageView(VK_NULL_HANDLE), textureSampler(VK_NULL_HANDLE) {};
        ~VKTextureHandle() override { destroy();};

        static VKTextureHandle* create(std::string filename, u32 magFilter, u32 minFilter, bool repeat, bool flip);
        void bind() override;
        void destroy() override;
    private:
        VkImage textureImage;
        VkDeviceMemory textureImageMemory;
        VkImageView textureImageView;
        VkSampler textureSampler;
    };
}
#endif