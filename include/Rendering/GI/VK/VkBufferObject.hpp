#pragma once
#if SDC_VULKAN
#include <vulkan/vulkan.h>
#include "Rendering/RenderTypes.hpp"

namespace GI::detail {
    class VKBufferObject final {
    public:
        VKBufferObject() : vertexBuffer(VK_NULL_HANDLE), vertexBufferMemory(VK_NULL_HANDLE), indexBuffer(VK_NULL_HANDLE), indexBufferMemory(VK_NULL_HANDLE){}
        ~VKBufferObject() { destroy(); }

        static VKBufferObject* create(const Stardust_Celeste::Rendering::Vertex* vert_data, size_t vert_size, const uint16_t* indices, size_t idx_size);
        void bind();
        void draw();

        void update(const Stardust_Celeste::Rendering::Vertex* vert_data, size_t vert_size, const uint16_t* indices, size_t idx_size);
        void destroy();

    private:
        VkBuffer vertexBuffer;
        VkDeviceMemory vertexBufferMemory;
        VkBuffer indexBuffer;
        VkDeviceMemory indexBufferMemory;
        size_t idx_count;
    };
}
#endif