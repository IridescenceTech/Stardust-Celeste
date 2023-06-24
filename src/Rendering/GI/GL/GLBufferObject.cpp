#include <Platform/Platform.hpp>
#include <Rendering/GI/GL/GLBufferObject.hpp>
#include "Rendering/RenderTypes.hpp"
#include <Utilities/Logger.hpp>
#define BUILD_PC (BUILD_PLAT == BUILD_WINDOWS || BUILD_PLAT == BUILD_POSIX)

#if BUILD_PC
namespace GI {
    extern GLuint programID;
}
#endif

namespace GI::detail{
    GLBufferObject* GLBufferObject::create(const Stardust_Celeste::Rendering::Vertex* vert_data, size_t vert_size, const uint16_t* indices, size_t idx_size) {
        GLBufferObject* vbo = new GLBufferObject();
        vbo->setup = false;

        vbo->update(vert_data, vert_size, indices, idx_size);
        return vbo;
    }
    GLBufferObject* GLBufferObject::create(const Stardust_Celeste::Rendering::SimpleVertex* vert_data, size_t vert_size, const uint16_t* indices, size_t idx_size) {
        GLBufferObject* vbo = new GLBufferObject();
        vbo->setup = false;

        vbo->update(vert_data, vert_size, indices, idx_size);
        return vbo;
    }

    void GLBufferObject::bind() {
        if(setup) {
#if BUILD_PC
            glBindVertexArray(vao);
#elif BUILD_PLAT == BUILD_VITA
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
#endif
        }
    }

    void GLBufferObject::draw(Rendering::PrimType p) {
        #if BUILD_PC
        if(simple) {
            glUniform1i(glGetUniformLocation(programID, "simple"), 1);
        } else {
            glUniform1i(glGetUniformLocation(programID, "simple"), 0);
        }
                if (p == Rendering::PrimType::PRIM_TYPE_TRIANGLE) {
                    glDrawElements(GL_TRIANGLES, idx_count, GL_UNSIGNED_SHORT,
                                   nullptr);
                } else {
                    glLineWidth(4.0f);
                    glDrawElements(GL_LINE_STRIP, idx_count, GL_UNSIGNED_SHORT,
                                   nullptr);
                }
        #elif BUILD_PLAT == BUILD_PSP
                sceGuShadeModel(GU_SMOOTH);
                if(!simple) {
                if (p == Rendering::PrimType::PRIM_TYPE_TRIANGLE) {
                    sceGumDrawArray(GU_TRIANGLES,
                                    GU_INDEX_16BIT | GU_TEXTURE_32BITF | GU_COLOR_8888 |
                                            GU_VERTEX_32BITF | GU_TRANSFORM_3D,
                                    idx_count, idx_buf, vtx_buf);
                } else {
                    sceGumDrawArray(GU_LINE_STRIP,
                                    GU_INDEX_16BIT | GU_TEXTURE_32BITF | GU_COLOR_8888 |
                                            GU_VERTEX_32BITF | GU_TRANSFORM_3D,
                                    idx_count, idx_buf, vtx_buf);
                }
                } else {
                    if (p == Rendering::PrimType::PRIM_TYPE_TRIANGLE) {
                    sceGumDrawArray(GU_TRIANGLES,
                                    GU_INDEX_16BIT | GU_TEXTURE_16BIT | GU_COLOR_8888 |
                                            GU_VERTEX_16BIT | GU_TRANSFORM_3D,
                                    idx_count, idx_buf, vtx_buf);
                    }
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
        #elif BUILD_PLAT == BUILD_3DS
                glEnableClientState(GL_VERTEX_ARRAY);
                glEnableClientState(GL_COLOR_ARRAY);
                glEnableClientState(GL_TEXTURE_COORD_ARRAY);

                glVertexPointer(3, GL_FLOAT, sizeof(T),
                                reinterpret_cast<uint8_t *>(vertices.data()) +
                                        (sizeof(float) * 3));
                glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(T),
                               reinterpret_cast<uint8_t *>(vertices.data()) +
                                       (sizeof(float) * 2));
                glTexCoordPointer(2, GL_FLOAT, sizeof(T), vertices.data());

                if (p == PRIM_TYPE_TRIANGLE) {
                    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT,
                                   indices.data());
                } else {
                    glDrawElements(GL_LINE_STRIP, indices.size(), GL_UNSIGNED_SHORT,
                                   indices.data());
                }

                glDisableClientState(GL_VERTEX_ARRAY);
                glDisableClientState(GL_COLOR_ARRAY);
                glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        #endif
    }

    void GLBufferObject::update(const Stardust_Celeste::Rendering::Vertex* vert_data, size_t vert_size, const uint16_t* indices, size_t idx_size) {
#if BUILD_PC
        simple = false;
        if (!setup) {
            glGenVertexArrays(1, &vao);
            glGenBuffers(1, &vbo);
        }
        bind();

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Stardust_Celeste::Rendering::Vertex) * vert_size,
                     vert_data, GL_STATIC_DRAW);

        const auto stride = sizeof(Stardust_Celeste::Rendering::Vertex);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride,
                              reinterpret_cast<void *>(sizeof(float) * 3));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, stride,
                              reinterpret_cast<void *>(sizeof(float) * 2));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, nullptr);

        if (!setup) {
            glGenBuffers(1, &ebo);
            setup = true;
        }
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u16) * idx_size,
                     indices, GL_STATIC_DRAW);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
#elif BUILD_PLAT == BUILD_VITA
        if (idx_size <= 0 || vert_size <= 0)
            return;

        if (!setup) {
            glGenBuffers(1, &vbo);
        }
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Stardust_Celeste::Rendering::Vertex) * vert_size,
                     vert_data, GL_STATIC_DRAW);

        if (!setup) {
            glGenBuffers(1, &ebo);
            setup = true;
        }
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u16) * idx_size,
                     indices, GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
#elif BUILD_PLAT == BUILD_PSP
        vtx_size = vert_size;
        vtx_buf = vert_data;
        idx_buf = indices;
        sceKernelDcacheWritebackInvalidateAll();
#endif
        idx_count = idx_size;
    }

    void GLBufferObject::update(const Stardust_Celeste::Rendering::SimpleVertex* vert_data, size_t vert_size, const uint16_t* indices, size_t idx_size) {
#if BUILD_PC
        simple = true;
        if (!setup) {
            glGenVertexArrays(1, &vao);
            glGenBuffers(1, &vbo);
        }
        bind();

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Stardust_Celeste::Rendering::SimpleVertex) * vert_size,
                     vert_data, GL_STATIC_DRAW);

        const auto stride = sizeof(Stardust_Celeste::Rendering::SimpleVertex);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_UNSIGNED_SHORT, GL_TRUE, stride,
                              reinterpret_cast<void *>(sizeof(uint16_t) * 2 + sizeof(uint32_t)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, stride,
                              reinterpret_cast<void *>(sizeof(uint16_t) * 2));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_UNSIGNED_SHORT, GL_TRUE, stride, nullptr);

        if (!setup) {
            glGenBuffers(1, &ebo);
            setup = true;
        }
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u16) * idx_size,
                     indices, GL_STATIC_DRAW);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
#elif BUILD_PLAT == BUILD_VITA
        if (idx_size <= 0 || vert_size <= 0)
            return;

        if (!setup) {
            glGenBuffers(1, &vbo);
        }
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Stardust_Celeste::Rendering::Vertex) * vert_size,
                     vert_data, GL_STATIC_DRAW);

        if (!setup) {
            glGenBuffers(1, &ebo);
            setup = true;
        }
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u16) * idx_size,
                     indices, GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
#elif BUILD_PLAT == BUILD_PSP
        vtx_size = vert_size;
        vtx_buf = vert_data;
        idx_buf = indices;
        sceKernelDcacheWritebackInvalidateAll();
        simple = true;
#endif
        idx_count = idx_size;
    }

    void GLBufferObject::destroy() {
        if(setup) {
#if BUILD_PC
            glDeleteVertexArrays(1, &vao);
            glDeleteBuffers(1, &vbo);
            glDeleteBuffers(1, &ebo);
            setup = false;

#elif BUILD_PLAT == BUILD_VITA
            if (idx_count <= 0)
                return;

            glDeleteBuffers(1, &vbo);
            glDeleteBuffers(1, &ebo);
            setup = false;
#endif
        }
    }
}