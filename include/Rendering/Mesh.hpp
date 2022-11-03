#pragma once

#include "Utilities/Assertion.hpp"
#include "Utilities/Logger.hpp"
#include "Utilities/Singleton.hpp"
#include "Utilities/Types.hpp"
#include <array>

#include "RenderTypes.hpp"

#if BUILD_PLAT == BUILD_PSP
#define PACKED __attribute__((__packed__))
#else
#define PACKED
#endif

#define BUILD_PC (BUILD_PLAT == BUILD_WINDOWS || BUILD_PLAT == BUILD_POSIX)

#if BUILD_PC
#include <glad/glad.hpp>
#elif BUILD_PLAT == BUILD_PSP
#include <intraFont.h>
#include <pspctrl.h>
#include <pspdebug.h>
#include <pspdisplay.h>
#include <pspge.h>
#include <pspgu.h>
#include <pspgum.h>
#include <pspkernel.h>
#include <psppower.h>
#include <psptypes.h>
#include <psputils.h>
#include <stdarg.h>
#elif BUILD_PLAT == BUILD_VITA
#include <vitaGL.h>
#endif

#if USE_EASTL
#include <EASTL/array.h>
#include <EASTL/vector.h>
#endif

#include <Rendering/RenderContext.hpp>

namespace Stardust_Celeste::Rendering {
/**
 * @brief Packed vertices
 */
struct PACKED Vertex {
    float u, v;
    Color color;
    float x, y, z;
};

// TODO: Optimized Vertex structure - u16 x,y,z - u16 color - u16 u,v
// TODO: Lit data structure including normals
// TODO: Look at this class again with FixedMesh

enum PrimType { PRIM_TYPE_TRIANGLE, PRIM_TYPE_LINE };

/**
 * @brief Mesh takes ownership of vertices and indices
 */
template <class T> class Mesh : public NonCopy {
  private:
#if BUILD_PC || BUILD_PLAT == BUILD_VITA
    GLuint vbo, vao, ebo;
    bool setup;
#endif

  public:
    Mesh()
#if BUILD_PC || BUILD_PLAT == BUILD_VITA
        : vbo(0), vao(0), ebo(0), setup(false)
#endif
    {
        vertices.clear();
        vertices.shrink_to_fit();
        indices.clear();
        indices.shrink_to_fit();
    };

    ~Mesh() { delete_data(); }

    // TODO: Vert type changes enabled attributes
    auto setup_buffer() -> void {
#if BUILD_PC

        if (!setup) {
            glGenVertexArrays(1, &vao);
            glGenBuffers(1, &vbo);
        }
        bind();

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(T) * vertices.size(),
                     vertices.data(), GL_STATIC_DRAW);

        const auto stride = sizeof(T);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride,
                              reinterpret_cast<void *>(sizeof(float) * 3));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_FALSE, stride,
                              reinterpret_cast<void *>(sizeof(float) * 2));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, nullptr);

        if (!setup) {
            glGenBuffers(1, &ebo);
            setup = true;
        }
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u16) * indices.size(),
                     indices.data(), GL_STATIC_DRAW);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

#elif BUILD_PLAT == BUILD_VITA
        if (indices.size() <= 0 || vertices.size() <= 0)
            return;

        if (!setup) {
            glGenBuffers(1, &vbo);
        }
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(T) * vertices.size(),
                     vertices.data(), GL_STATIC_DRAW);

        if (!setup) {
            glGenBuffers(1, &ebo);
            setup = true;
        }
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u16) * indices.size(),
                     indices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
#elif BUILD_PLAT == BUILD_PSP
        sceKernelDcacheWritebackInvalidateAll();
#endif
    }

    auto clear_data() -> void {
        vertices.clear();
        indices.clear();
        vertices.shrink_to_fit();
        indices.shrink_to_fit();
    }

    auto delete_data() -> void {

#if BUILD_PC
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &ebo);
        setup = false;
#elif BUILD_PLAT == BUILD_VITA
        if (indices.size() <= 0)
            return;

        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &ebo);
        setup = false;
#endif
    }

    // TODO: Vert type changes enabled attributes
    auto draw(PrimType p = PRIM_TYPE_TRIANGLE) -> void {
        bind();

        Rendering::RenderContext::get().set_matrices();

#if BUILD_PC
        // TODO: Bind Program
        if (p == PRIM_TYPE_TRIANGLE) {
            glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT,
                           nullptr);
        } else {
            glDrawElements(GL_LINE_STRIP, indices.size(), GL_UNSIGNED_SHORT,
                           nullptr);
        }
#elif BUILD_PLAT == BUILD_PSP
        sceGuShadeModel(GU_SMOOTH);
        if (p == PRIM_TYPE_TRIANGLE) {
            sceGumDrawArray(GU_TRIANGLES,
                            GU_INDEX_16BIT | GU_TEXTURE_32BITF | GU_COLOR_8888 |
                                GU_VERTEX_32BITF | GU_TRANSFORM_3D,
                            indices.size(), indices.data(), vertices.data());
        } else {
            sceGumDrawArray(GU_LINE_STRIP,
                            GU_INDEX_16BIT | GU_TEXTURE_32BITF | GU_COLOR_8888 |
                                GU_VERTEX_32BITF | GU_TRANSFORM_3D,
                            indices.size(), indices.data(), vertices.data());
        }
#elif BUILD_PLAT == BUILD_VITA
        if (vertices.size() == 0 || indices.size() == 0)
            return;

        const auto stride = sizeof(T);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride,
                              reinterpret_cast<void *>(sizeof(float) * 3));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, stride,
                              reinterpret_cast<void *>(sizeof(float) * 2));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, nullptr);

        if (p == PRIM_TYPE_TRIANGLE) {
            glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT,
                           nullptr);
        } else {
            glDrawElements(GL_LINE_STRIP, indices.size(), GL_UNSIGNED_SHORT,
                           nullptr);
        }

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
#endif
    }

    auto bind() -> void {
#if BUILD_PC
        glBindVertexArray(vao);
#elif BUILD_PLAT == BUILD_VITA
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
#endif
    }

    inline auto get_index_count() -> s32 { return indices.size(); }
#if USE_EASTL
    eastl::vector<T> vertices;
    eastl::vector<u16> indices;
#else
    std::vector<T> vertices;
    std::vector<u16> indices;
#endif
};

template <class T, size_t V, size_t I> class FixedMesh : public NonCopy {
  private:
#if BUILD_PC || BUILD_PLAT == BUILD_VITA
    GLuint vbo, vao, ebo;
    bool setup;
#endif

  public:
    FixedMesh()
#if BUILD_PC || BUILD_PLAT == BUILD_VITA
        : vbo(0), vao(0), ebo(0), setup(false)
#endif
    {
        for (int i = 0; i < V; i++) {
            vertices[i] = {0};
        }
        for (int i = 0; i < I; i++) {
            indices[i] = 0;
        }
    };

    ~FixedMesh() { delete_data(); }

    // TODO: Vert type changes enabled attributes
    auto setup_buffer() -> void {
#if BUILD_PC
        if (!setup) {
            glGenVertexArrays(1, &vao);
            glGenBuffers(1, &vbo);
        }
        bind();

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(T) * vertices.size(),
                     vertices.data(), GL_STATIC_DRAW);

        const auto stride = sizeof(T);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride,
                              reinterpret_cast<void *>(sizeof(float) * 3));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_FALSE, stride,
                              reinterpret_cast<void *>(sizeof(float) * 2));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, nullptr);

        if (!setup) {
            glGenBuffers(1, &ebo);
            setup = true;
        }

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u16) * indices.size(),
                     indices.data(), GL_STATIC_DRAW);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

#elif BUILD_PLAT == BUILD_VITA
        if (indices.size() <= 0 || vertices.size() <= 0)
            return;

        if (!setup) {
            glGenBuffers(1, &vbo);
        }

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(T) * vertices.size(),
                     vertices.data(), GL_STATIC_DRAW);

        if (!setup) {
            glGenBuffers(1, &ebo);
            setup = true;
        }

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u16) * indices.size(),
                     indices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
#elif BUILD_PLAT == BUILD_PSP
        sceKernelDcacheWritebackInvalidateAll();
#endif
    }

    auto clear_data() -> void {
        for (int i = 0; i < V; i++) {
            vertices[i] = {0};
        }
        for (int i = 0; i < I; i++) {
            indices[i] = 0;
        }
    }

    auto delete_data() -> void {
#if BUILD_PC
        if (!setup)
            return;
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &ebo);
        setup = false;
#elif BUILD_PLAT == BUILD_VITA
        if (!setup)
            return;
        if (indices.size() <= 0)
            return;

        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &ebo);
        setup = false;
#endif
    }

    // TODO: Optional drawing modes for draw()
    // TODO: Vert type changes enabled attributes
    auto draw() -> void {
#ifndef PSP
        if (!setup)
            return;
#endif

        bind();

        Rendering::RenderContext::get().set_matrices();

#if BUILD_PC
        // TODO: Bind Program
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT,
                       nullptr);
#elif BUILD_PLAT == BUILD_PSP
        sceGuShadeModel(GU_SMOOTH);
        sceGumDrawArray(GU_TRIANGLES,
                        GU_INDEX_16BIT | GU_TEXTURE_32BITF | GU_COLOR_8888 |
                            GU_VERTEX_32BITF | GU_TRANSFORM_3D,
                        indices.size(), indices.data(), vertices.data());
#elif BUILD_PLAT == BUILD_VITA
        if (vertices.size() == 0 || indices.size() == 0)
            return;

        const auto stride = sizeof(T);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride,
                              reinterpret_cast<void *>(sizeof(float) * 3));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, stride,
                              reinterpret_cast<void *>(sizeof(float) * 2));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, nullptr);

        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT,
                       nullptr);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
#endif
    }

    auto bind() -> void {
#if BUILD_PC
        glBindVertexArray(vao);
#elif BUILD_PLAT == BUILD_VITA
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
#endif
    }

    inline auto get_index_count() -> s32 { return indices.size(); }

#if USE_EASTL
    eastl::array<T, V> vertices;
    eastl::array<u16, I> indices;
#else
    std::array<T, V> vertices;
    std::array<u16, I> indices;
#endif
};

} // namespace Stardust_Celeste::Rendering