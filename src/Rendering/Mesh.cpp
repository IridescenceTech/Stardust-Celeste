#include <Rendering/Mesh.hpp>

#define BUILD_PC (BUILD_PLAT == BUILD_WINDOWS || BUILD_PLAT == BUILD_POSIX)

#if BUILD_PC
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#elif BUILD_PSP
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
    Mesh::Mesh(Vertex* vertices, u16* indices, size_t idxc) {
        add_data(vertices, indices, idxc);
    }

    Mesh::~Mesh() {
        delete_data();
    }

    auto Mesh::add_data(Vertex* vertices, u16* indices, size_t idxc) -> void {
        vert_data = vertices;
        idx_data = indices;
        idx_count = idxc;

#if BUILD_PC
        //TODO: Setup GL Buffers
#endif
    }

    auto Mesh::delete_data() -> void {
        delete vert_data;
        delete idx_data;
        idx_count = 0;

#if BUILD_PC
        //TODO: Delete GL Buffers
#endif
    }

    auto Mesh::draw() -> void {
        bind();

#if BUILD_PC
        //TODO: Bind Program
        //TODO: Draw
#else
        //TODO: Draw PSP
#endif
    }

    auto Mesh::bind() -> void {
#if BUILD_PC
        //TODO: Bind GL Buffers
#endif
    }
}