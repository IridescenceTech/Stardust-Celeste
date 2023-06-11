#pragma once
#include <glad/glad.hpp>
#include <Rendering/GI/BufferObject.hpp>
#include <Rendering/RenderTypes.hpp>

namespace GI::detail {
    using namespace Stardust_Celeste;
    class GLBufferObject final : public BufferObject {
    public:
        GLBufferObject() : vbo(0), vao(0), ebo(0), setup(false), idx_count(0) {}
        ~GLBufferObject() { destroy(); }

        static GLBufferObject* create(const Stardust_Celeste::Rendering::Vertex* vert_data, size_t vert_size, const uint16_t* indices, size_t idx_size);
        static GLBufferObject* create(const Stardust_Celeste::Rendering::SimpleVertex* vert_data, size_t vert_size, const uint16_t* indices, size_t idx_size);
        void bind() override;
        void draw(Rendering::PrimType p) override;

        void update(const Stardust_Celeste::Rendering::Vertex* vert_data, size_t vert_size, const uint16_t* indices, size_t idx_size) override;
        void update(const Stardust_Celeste::Rendering::SimpleVertex* vert_data, size_t vert_size, const uint16_t* indices, size_t idx_size) override;
        void destroy() override;

    private:
        GLuint vbo, vao, ebo;
        bool setup;
        size_t idx_count;
    };
}