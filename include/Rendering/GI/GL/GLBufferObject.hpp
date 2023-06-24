#pragma once
#if BUILD_PLAT == BUILD_WINDOWS || BUILD_PLAT == BUILD_POSIX
#include <glad/glad.hpp>
#endif

#if PSP
#include <gu2gl/gu2gl.h>
#endif
#include <Rendering/GI/BufferObject.hpp>
#include <Rendering/RenderTypes.hpp>

namespace GI::detail {
    using namespace Stardust_Celeste;
    class GLBufferObject final : public BufferObject {
    public:
        GLBufferObject() :
#if BUILD_PLAT == BUILD_WINDOWS || BUILD_PLAT == BUILD_POSIX
        vbo(0), vao(0), ebo(0),
#endif
        setup(false), idx_count(0) {}
        ~GLBufferObject() { destroy(); }

        static GLBufferObject* create(const Stardust_Celeste::Rendering::Vertex* vert_data, size_t vert_size, const uint16_t* indices, size_t idx_size);
        static GLBufferObject* create(const Stardust_Celeste::Rendering::SimpleVertex* vert_data, size_t vert_size, const uint16_t* indices, size_t idx_size);
        void bind() override;
        void draw(Rendering::PrimType p) override;

        void update(const Stardust_Celeste::Rendering::Vertex* vert_data, size_t vert_size, const uint16_t* indices, size_t idx_size) override;
        void update(const Stardust_Celeste::Rendering::SimpleVertex* vert_data, size_t vert_size, const uint16_t* indices, size_t idx_size) override;
        void destroy() override;

    private:
#if BUILD_PLAT == BUILD_WINDOWS || BUILD_PLAT == BUILD_POSIX
        GLuint vbo, vao, ebo;
#endif
#if BUILD_PLAT == BUILD_PSP
        size_t vtx_size;
        const void* vtx_buf;
        const void* idx_buf;
#endif
        bool simple = false;
        bool setup;
        size_t idx_count;
    };
}