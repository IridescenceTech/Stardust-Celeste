#pragma once
#include <string>
#include "Rendering/RenderTypes.hpp"

namespace GI {
    class TextureHandle {
    public:
        TextureHandle() = default;
        virtual ~TextureHandle() = default;

        virtual void bind() = 0;
        virtual void destroy() = 0;

        uint32_t id;
    };
}