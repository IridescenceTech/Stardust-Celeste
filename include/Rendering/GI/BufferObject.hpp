#pragma once
#include "Rendering/RenderTypes.hpp"

namespace GI {
    using namespace Stardust_Celeste;
    class BufferObject {
    public:
        BufferObject() = default;
        virtual ~BufferObject() = default;

        virtual void bind() = 0;
        virtual void draw(Rendering::PrimType p) = 0;

        virtual void update(const Stardust_Celeste::Rendering::Vertex* vert_data, size_t vert_size, const uint16_t* indices, size_t idx_size) = 0;
        virtual void destroy() = 0;
    };
}