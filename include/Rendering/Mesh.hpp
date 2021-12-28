/**
 * @file RenderContext.hpp
 * @author Nathan Bourgeois <iridescentrosesfall@gmail.com>
 * @brief Rendering Context
 * @version 0.1
 * @date 2021-12-26
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#pragma once

#include "Utilities/Singleton.hpp"
#include "Utilities/Logger.hpp"
#include "Utilities/Assertion.hpp"
#include "Utilities/Types.hpp"

#include "RenderTypes.hpp"

#if BUILD_PLAT == BUILD_PSP
#define PACKED __attribute__((__packed__))
#else
#define PACKED 
#endif


#define BUILD_PC (BUILD_PLAT == BUILD_WINDOWS || BUILD_PLAT == BUILD_POSIX)

#if BUILD_PC
#include <glad/glad.h>
#elif BUILD_PLAT == BUILD_PSP
#include <pspgu.h>
#include <pspgum.h>
#include <pspctrl.h>
#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspdebug.h>
#include <psppower.h>
#include <psptypes.h>
#include <pspge.h>
#include <psputils.h>
#include <stdarg.h>
#include <intraFont.h>
#endif


namespace Stardust_Celeste::Rendering
{
    /**
     * @brief Packed vertices
     */
    struct PACKED Vertex {
        float u, v;
        Color color;
        float x, y, z;
    };

    //TODO: Optimized Vertex structure - u16 x,y,z - u16 color - u16 u,v
    //TODO: Lit data structure including normals

    /**
     * @brief Mesh takes ownership of vertices and indices
     */
    class Mesh : public NonCopy {
    public:
        Mesh() = default;

        Mesh(Vertex* vertices, u16* indices, size_t idx_count) {
            add_data(vertices, indices, idx_count);
        }

        ~Mesh() {
            delete_data();
        }


    auto add_data(Vertex* vertices, u16* indices, size_t idxc) -> void {
        vert_data = vertices;
        idx_data = indices;
        idx_count = idxc;

#if BUILD_PC
        //TODO: Setup GL Buffers
#endif
    }

    auto delete_data() -> void {
        delete vert_data;
        delete idx_data;
        idx_count = 0;

#if BUILD_PC
        //TODO: Delete GL Buffers
#endif
    }

    auto draw() -> void {
        bind();

#if BUILD_PC
        //TODO: Bind Program
        //TODO: Draw
#else
        //TODO: Draw PSP
        sceGuShadeModel(GU_SMOOTH);
        sceGumDrawArray(GU_TRIANGLES, GU_INDEX_16BIT | GU_TEXTURE_32BITF | GU_COLOR_8888 | GU_VERTEX_32BITF | GU_TRANSFORM_3D, idx_count, idx_data, vert_data);
#endif
    }

    auto bind() -> void {
#if BUILD_PC
        //TODO: Bind GL Buffers
#endif
    }

        inline auto get_index_count() -> s32 {
            return idx_count;
        }

    private:
        size_t idx_count = 0;
        Vertex* vert_data = nullptr;
        u16* idx_data = nullptr;
    };

}