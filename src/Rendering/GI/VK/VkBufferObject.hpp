#pragma once
#include <Rendering/Mesh.hpp>
#include <vulkan/vulkan.h>

namespace GI::detail {
    class VKBufferObject final {
    public:
        VKBufferObject() : vertexBuffer(VK_NULL_HANDLE), vertexBufferMemory(VK_NULL_HANDLE), indexBuffer(VK_NULL_HANDLE), indexBufferMemory(VK_NULL_HANDLE){}
        ~VKBufferObject() { destroy(); }

        static VKBufferObject* create(const std::vector<Stardust_Celeste::Rendering::Vertex>& vertices, const std::vector<uint16_t>& indices);
        void bind();
        void draw();

        void update();
        void destroy();

    private:
        VkBuffer vertexBuffer;
        VkDeviceMemory vertexBufferMemory;
        VkBuffer indexBuffer;
        VkDeviceMemory indexBufferMemory;
        size_t idx_count;
    };
}