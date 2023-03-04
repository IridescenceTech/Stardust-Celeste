#pragma once
#ifndef NO_EXPERIMENTAL_GRAPHICS
#include <vulkan/vulkan.h>
#include <Rendering/GI/BufferObject.hpp>
#include "Rendering/RenderTypes.hpp"

namespace GI::detail {
    class VKBufferObject final : public BufferObject {
    public:
        VKBufferObject() : vertexBuffer(VK_NULL_HANDLE), vertexBufferMemory(VK_NULL_HANDLE), indexBuffer(VK_NULL_HANDLE), indexBufferMemory(VK_NULL_HANDLE), setup(false) {}
        ~VKBufferObject() { destroy(); }

        static VKBufferObject* create(const Stardust_Celeste::Rendering::Vertex* vert_data, size_t vert_size, const uint16_t* indices, size_t idx_size);
        void bind() override;
        void draw(Rendering::PrimType p) override;

        void update(const Stardust_Celeste::Rendering::Vertex* vert_data, size_t vert_size, const uint16_t* indices, size_t idx_size) override;
        void destroy() override;

    private:
        VkBuffer vertexBuffer;
        VkDeviceMemory vertexBufferMemory;
        VkBuffer indexBuffer;
        VkDeviceMemory indexBufferMemory;
        size_t idx_count;
        bool setup;
    };
}
#endif