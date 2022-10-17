#pragma once

#include "Utilities/Assertion.hpp"
#include "Utilities/Logger.hpp"
#include "Utilities/Singleton.hpp"
#include "Utilities/Types.hpp"

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

/**
 * @brief Mesh takes ownership of vertices and indices
 */
class Mesh : public NonCopy {
  private:
#if BUILD_PC || BUILD_PLAT == BUILD_VITA
    GLuint vbo, vao, ebo;
#endif

  public:
    Mesh()
#if BUILD_PC || BUILD_PLAT == BUILD_VITA
        : vbo(0), vao(0), ebo(0)
#endif
                              {};

    Mesh(Vertex *vertices, size_t vcount, u16 *indices, size_t idx_count) {
        add_data(vertices, vcount, indices, idx_count);
    }

    ~Mesh() { delete_data(); }

    auto add_data(Vertex *vertices, size_t vcount, u16 *indices, size_t idxc)
        -> void {
        vert_data = vertices;
        idx_data = indices;
        idx_count = idxc;

#if BUILD_PC
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        bind();

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vcount, vert_data,
                     GL_STATIC_DRAW);

        const auto stride = sizeof(Vertex);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride,
                              reinterpret_cast<void *>(sizeof(float) * 3));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_FALSE, stride,
                              reinterpret_cast<void *>(sizeof(float) * 2));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, nullptr);

        glGenBuffers(1, &ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u16) * idx_count, idx_data,
                     GL_STATIC_DRAW);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

#elif BUILD_PLAT == BUILD_VITA
        if (idxc <= 0 || vcount <= 0)
            return;

        glGenBuffers(1, &vbo);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vcount, vert_data,
                     GL_STATIC_DRAW);

        glGenBuffers(1, &ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u16) * idx_count, idx_data,
                     GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
#elif BUILD_PLAT == BUILD_PSP
        sceKernelDcacheWritebackInvalidateAll();
#endif
    }

    auto delete_data() -> void {
        vert_data = NULL;
        idx_data = NULL;

#if BUILD_PC
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &ebo);
#elif BUILD_PLAT == BUILD_VITA
        if (idx_count <= 0)
            return;

        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &ebo);
#endif

        idx_count = 0;
    }

    auto draw() -> void {
        bind();

        Rendering::RenderContext::get().set_matrices();

#if BUILD_PC
        // TODO: Bind Program
        glDrawElements(GL_TRIANGLES, idx_count, GL_UNSIGNED_SHORT, nullptr);
#elif BUILD_PLAT == BUILD_PSP
        sceGuShadeModel(GU_SMOOTH);
        sceGumDrawArray(GU_TRIANGLES,
                        GU_INDEX_16BIT | GU_TEXTURE_32BITF | GU_COLOR_8888 |
                            GU_VERTEX_32BITF | GU_TRANSFORM_3D,
                        idx_count, idx_data, vert_data);
#elif BUILD_PLAT == BUILD_VITA
        if (vert_data == NULL || idx_data == NULL)
            return;

        const auto stride = sizeof(Vertex);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride,
                              reinterpret_cast<void *>(sizeof(float) * 3));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, stride,
                              reinterpret_cast<void *>(sizeof(float) * 2));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, nullptr);

        glDrawElements(GL_TRIANGLES, idx_count, GL_UNSIGNED_SHORT, nullptr);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
#endif
    }

    auto draw_lines() -> void {
        bind();

        Rendering::RenderContext::get().set_matrices();

#if BUILD_PC
        // TODO: Bind Program
        glLineWidth(2.0f);
        glDisable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);

        glDrawElements(GL_LINE_STRIP, idx_count, GL_UNSIGNED_SHORT, nullptr);

        glEnable(GL_TEXTURE_2D);
#elif BUILD_PLAT == BUILD_PSP
        sceGuShadeModel(GU_SMOOTH);
        sceGuDisable(GU_TEXTURE_2D);

        sceGumDrawArray(GU_LINE_STRIP,
                        GU_INDEX_16BIT | GU_TEXTURE_32BITF | GU_COLOR_8888 |
                            GU_VERTEX_32BITF | GU_TRANSFORM_3D,
                        idx_count, idx_data, vert_data);

        sceGuEnable(GU_TEXTURE_2D);
#elif BUILD_PLAT == BUILD_VITA
        glLineWidth(2.0f);
        glDisable(GL_TEXTURE_2D);

        if (vert_data == NULL || idx_data == NULL)
            return;

        const auto stride = sizeof(Vertex);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride,
                              reinterpret_cast<void *>(sizeof(float) * 3));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, stride,
                              reinterpret_cast<void *>(sizeof(float) * 2));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, nullptr);

        glDrawElements(GL_LINE_STRIP, idx_count, GL_UNSIGNED_SHORT, nullptr);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glEnable(GL_TEXTURE_2D);
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

    inline auto get_index_count() -> s32 { return idx_count; }

  private:
    size_t idx_count = 0;
    Vertex *vert_data = nullptr;
    u16 *idx_data = nullptr;
};

} // namespace Stardust_Celeste::Rendering