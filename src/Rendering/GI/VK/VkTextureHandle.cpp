#ifndef NO_EXPERIMENTAL_GRAPHICS
#include <Rendering/GI/VK/VkTextureHandle.hpp>
#include <Rendering/GI/VK/VkUtil.hpp>
#include <Rendering/Texture.hpp>

namespace GI::detail {
    static int handle_number = 0;

    VKTextureHandle* VKTextureHandle::create(std::string filename, u32 magFilter, u32 minFilter, bool repeat, bool flip) {
        VKTextureHandle* tex = new VKTextureHandle();

        int texWidth, texHeight, texChannels;
        stbi_set_flip_vertically_on_load(flip);
        stbi_uc* pixels = stbi_load(filename.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
        VkDeviceSize imageSize = texWidth * texHeight * 4;

        if (!pixels) {
            throw std::runtime_error("failed to load texture image!");
        }

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

        void* data;
        vkMapMemory(VKContext::get().logicalDevice, stagingBufferMemory, 0, imageSize, 0, &data);
        memcpy(data, pixels, static_cast<size_t>(imageSize));
        vkUnmapMemory(VKContext::get().logicalDevice, stagingBufferMemory);

        stbi_image_free(pixels);

        createImage(texWidth, texHeight, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, tex->textureImage, tex->textureImageMemory);

        transitionImageLayout(tex->textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        copyBufferToImage(stagingBuffer, tex->textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
        transitionImageLayout(tex->textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        vkDestroyBuffer(VKContext::get().logicalDevice, stagingBuffer, nullptr);
        vkFreeMemory(VKContext::get().logicalDevice, stagingBufferMemory, nullptr);

        tex->textureImageView = createImageView(tex->textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);

        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;

        VkFilter magf = VK_FILTER_NEAREST;
        VkFilter minf = VK_FILTER_NEAREST;
        if(magFilter == SC_TEX_FILTER_NEAREST) {
            magf = VK_FILTER_NEAREST;
        } else {
            magf = VK_FILTER_LINEAR;
        }
        if(minFilter == SC_TEX_FILTER_NEAREST) {
            minf = VK_FILTER_NEAREST;
        } else {
            minf = VK_FILTER_LINEAR;
        }


        samplerInfo.magFilter = magf;
        samplerInfo.minFilter = minf;

        if(repeat){
            samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
            samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
            samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        } else {
            samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
            samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
            samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        }
        samplerInfo.anisotropyEnable = VK_TRUE;

        VkPhysicalDeviceProperties properties{};
        vkGetPhysicalDeviceProperties(VKContext::get().physicalDevice, &properties);
        samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
        samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.mipLodBias = 0.0f;
        samplerInfo.minLod = 0.0f;
        samplerInfo.maxLod = 0.0f;

        if (vkCreateSampler(VKContext::get().logicalDevice, &samplerInfo, nullptr, &tex->textureSampler) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create texture sampler!");
        }

        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = tex->textureImageView;
        imageInfo.sampler = tex->textureSampler;

        // UPDATE DESCRIPTORS
        VkWriteDescriptorSet descriptorWrite{};
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.dstSet = VKPipeline::get().descriptorSet;
        descriptorWrite.dstBinding = 1;
        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrite.dstArrayElement = handle_number++;
        tex->id = descriptorWrite.dstArrayElement;
        SC_CORE_INFO("ADDED DESCRIPTOR {} {}", descriptorWrite.dstArrayElement, filename);

        descriptorWrite.pImageInfo = &imageInfo;

        vkUpdateDescriptorSets(VKContext::get().logicalDevice, 1, &descriptorWrite, 0, nullptr);

        return tex;
    }

    void VKTextureHandle::bind() {
        VKPipeline::get().bindTextureID(id);
    }

    void VKTextureHandle::destroy() {
        if(textureSampler != VK_NULL_HANDLE) {
            vkDestroySampler(VKContext::get().logicalDevice, textureSampler, nullptr);
            textureSampler = VK_NULL_HANDLE;
        }

        if(textureImageView != VK_NULL_HANDLE) {
            vkDestroyImageView(VKContext::get().logicalDevice, textureImageView, nullptr);
            textureImageView = VK_NULL_HANDLE;
        }

        if(textureImage != VK_NULL_HANDLE) {
            vkDestroyImage(VKContext::get().logicalDevice, textureImage, nullptr);
            textureImage = VK_NULL_HANDLE;
        }

        if(textureImageMemory != VK_NULL_HANDLE) {
            vkFreeMemory(VKContext::get().logicalDevice, textureImageMemory, nullptr);
            textureImageMemory = VK_NULL_HANDLE;
        }
    }

}
#endif