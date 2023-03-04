#pragma once
#ifndef NO_EXPERIMENTAL_GRAPHICS

#include <vulkan/vulkan.h>
#include "Utilities/Assertion.hpp"

namespace GI::detail{

    struct UniformBufferObject {
        glm::mat4 model;
        glm::mat4 projview;
    };

    class VKPipeline : public Singleton{
    public:
        inline static auto get() -> VKPipeline& {
            static VKPipeline vkp;
            return vkp;
        }

        void init();
        void beginFrame();
        void endFrame();
        void deinit();

        void bindTextureID(uint32_t id);

        void updateDescriptorSet();
        void updateUniformBuffer();

        UniformBufferObject ubo{};

        VkRenderPass renderPass;
        VkDescriptorSetLayout descriptorSetLayout;

        VkPipelineLayout pipelineLayout;
        VkPipeline graphicsPipeline;

        VkCommandPool commandPool;

        VkImage depthImage;
        VkDeviceMemory depthImageMemory;
        VkImageView depthImageView;

        VkBuffer uniformBuffer;
        VkDeviceMemory uniformBufferMemory;
        void* uniformBufferMapped;

        VkDescriptorPool descriptorPool;
        VkDescriptorSet descriptorSet;

        std::vector<VkFramebuffer> swapChainFramebuffers;

        VkCommandBuffer commandBuffer;
        VkSemaphore imageAvailableSemaphores;
        VkSemaphore renderFinishedSemaphores;
        VkFence inFlightFence;

        glm::vec4 clearColor;
    private:

    };
}
#endif