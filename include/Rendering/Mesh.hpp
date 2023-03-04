#pragma once

#include "Utilities/Assertion.hpp"
#include "Utilities/Logger.hpp"
#include "Utilities/Singleton.hpp"
#include "Utilities/Types.hpp"
#include <array>

#include "RenderTypes.hpp"

#define BUILD_PC (BUILD_PLAT == BUILD_WINDOWS || BUILD_PLAT == BUILD_POSIX)

#if BUILD_PC

#ifndef NO_EXPERIMENTAL_GRAPHICS
#include <vulkan/vulkan.h>
#include <Rendering/GI/VK/VkContext.hpp>
#include <Rendering/GI/VK/VkPipeline.hpp>
#include <Rendering/GI/VK/VkBufferObject.hpp>
#endif
#include <glad/glad.hpp>

#elif BUILD_PLAT == BUILD_PSP
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
#elif BUILD_PLAT == BUILD_3DS
#include <GL/picaGL.h>
#endif

#if USE_EASTL
#include <EASTL/array.h>
#include <EASTL/vector.h>
#endif

#include <Rendering/RenderContext.hpp>

namespace Stardust_Celeste::Rendering {

// TODO: Optimized Vertex structure - u16 x,y,z - u16 color - u16 u,v
// TODO: Lit data structure including normals

/**
 * @brief Mesh takes ownership of vertices and indices
 */
template <class T> class Mesh : public NonCopy {
  private:
      GI::BufferObject* vbo;

  public:
    Mesh() : vbo(nullptr) {
        vertices.clear();
        vertices.shrink_to_fit();
        indices.clear();
        indices.shrink_to_fit();
    };

    ~Mesh() { delete_data(); }

    // TODO: Vert type changes enabled attributes
    auto setup_buffer() -> void {
        if(vbo == nullptr)
            vbo = GI::create_vertexbuffer(vertices.data(), vertices.size(), indices.data(), indices.size());
        else
            vbo->update(vertices.data(), vertices.size(), indices.data(), indices.size());
    }

    auto clear_data() -> void {
        vertices.clear();
        indices.clear();
        vertices.shrink_to_fit();
        indices.shrink_to_fit();
    }

    auto delete_data() -> void {
        if(vbo != nullptr) {
            delete vbo;
            vbo = nullptr;
        }
    }

    // TODO: Vert type changes enabled attributes
    auto draw(PrimType p = PRIM_TYPE_TRIANGLE) -> void {
        if(vbo != nullptr) {
            vbo->bind();
            Rendering::RenderContext::get().set_matrices();

            vbo->draw(p);
        }
    }

    auto bind() -> void {
        if(vbo != nullptr) {
            vbo->bind();
        }
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
      GI::BufferObject* vbo;

  public:
    FixedMesh() : vbo(nullptr) {
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
        if(vbo == nullptr)
            vbo = GI::create_vertexbuffer(vertices.data(), vertices.size(), indices.data(), indices.size());
        else
            vbo->update(vertices.data(), vertices.size(), indices.data(), indices.size());
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
        if(vbo != nullptr) {
            delete vbo;
            vbo = nullptr;
        }
    }

    // TODO: Vert type changes enabled attributes
    auto draw(PrimType p = PRIM_TYPE_TRIANGLE) -> void {
        if(vbo != nullptr) {
            vbo->bind();

            Rendering::RenderContext::get().set_matrices();

            vbo->draw(p);
        }
    }

    auto bind() -> void {
        if(vbo != nullptr) {
            vbo->bind();
        }
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