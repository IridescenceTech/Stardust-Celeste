#pragma once
#include <glad/glad.hpp>
#include <Rendering/GI/TextureHandle.hpp>
#include "Rendering/RenderTypes.hpp"

namespace GI::detail {
    class GLTextureHandle final : public TextureHandle {
    public:
        GLTextureHandle() {};
        ~GLTextureHandle() override { destroy();};

        static GLTextureHandle* create(std::string filename, u32 magFilter, u32 minFilter, bool repeat, bool flip);
        static GLTextureHandle* create_ram(uint8_t* buf, size_t len, u32 magFilter, u32 minFilter, bool repeat, bool flip);
        void bind() override;
        void destroy() override;
    };
}