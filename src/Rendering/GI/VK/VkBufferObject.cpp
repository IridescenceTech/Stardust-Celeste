#include "VkBufferObject.hpp"
#include "VkUtil.hpp"

namespace GI::detail {
    VKBufferObject* VKBufferObject::create(const std::vector<Stardust_Celeste::Rendering::Vertex>& vertices, const std::vector<uint16_t>& indices) {
        VKBufferObject* vbo = new VKBufferObject();

        {
            VkDeviceSize bufferSize = sizeof(Stardust_Celeste::Rendering::Vertex) * vertices.size();

            VkBuffer stagingBuffer;
            VkDeviceMemory stagingBufferMemory;
            createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

            void *data;
            vkMapMemory(VKContext::get().logicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
            memcpy(data, vertices.data(), (size_t) bufferSize);
            vkUnmapMemory(VKContext::get().logicalDevice, stagingBufferMemory);

            createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vbo->vertexBuffer, vbo->vertexBufferMemory);

            copyBuffer(stagingBuffer, vbo->vertexBuffer, bufferSize);

            vkDestroyBuffer(VKContext::get().logicalDevice, stagingBuffer, nullptr);
            vkFreeMemory(VKContext::get().logicalDevice, stagingBufferMemory, nullptr);
        }
        {
            VkDeviceSize bufferSize = sizeof(uint16_t) * indices.size();

            VkBuffer stagingBuffer;
            VkDeviceMemory stagingBufferMemory;
            createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

            void *data;
            vkMapMemory(VKContext::get().logicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
            memcpy(data, indices.data(), (size_t) bufferSize);
            vkUnmapMemory(VKContext::get().logicalDevice, stagingBufferMemory);

            createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vbo->indexBuffer, vbo->indexBufferMemory);

            copyBuffer(stagingBuffer, vbo->indexBuffer, bufferSize);

            vkDestroyBuffer(VKContext::get().logicalDevice, stagingBuffer, nullptr);
            vkFreeMemory(VKContext::get().logicalDevice, stagingBufferMemory, nullptr);
        }

        vbo->idx_count = indices.size();
        return vbo;
    }
    void VKBufferObject::update() {

    }

    void VKBufferObject::draw() {
        VKPipeline::get().updateUniformBuffer();

        auto buf = VKPipeline::get().commandBuffer;
        vkCmdBindDescriptorSets(buf, VK_PIPELINE_BIND_POINT_GRAPHICS, VKPipeline::get().pipelineLayout, 0, 1, &VKPipeline::get().descriptorSet, 0, nullptr);
        vkCmdDrawIndexed(buf, static_cast<uint32_t>(idx_count), 1, 0, 0, 0);
    }

    void VKBufferObject::bind() {
        VkBuffer vertexBuffers[] = {vertexBuffer};
        VkDeviceSize offsets[] = {0};

        auto buf = VKPipeline::get().commandBuffer;
        vkCmdBindVertexBuffers(buf, 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(buf, indexBuffer, 0, VK_INDEX_TYPE_UINT16);
    }

    void VKBufferObject::destroy() {
        if(indexBuffer != VK_NULL_HANDLE) {
            vkDestroyBuffer(VKContext::get().logicalDevice, indexBuffer, nullptr);
            indexBuffer = VK_NULL_HANDLE;
        }

        if(indexBufferMemory != VK_NULL_HANDLE) {
            vkFreeMemory(VKContext::get().logicalDevice, indexBufferMemory, nullptr);
            indexBufferMemory = VK_NULL_HANDLE;
        }

        if(vertexBuffer != VK_NULL_HANDLE) {
            vkDestroyBuffer(VKContext::get().logicalDevice, vertexBuffer, nullptr);
            vertexBuffer = VK_NULL_HANDLE;
        }

        if(vertexBufferMemory != VK_NULL_HANDLE) {
            vkFreeMemory(VKContext::get().logicalDevice, vertexBufferMemory, nullptr);
            vertexBufferMemory = VK_NULL_HANDLE;
        }
    }
}