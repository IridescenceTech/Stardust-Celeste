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

namespace Stardust_Celeste::Rendering
{
    /**
     * @brief Packed vertices
     */
    struct __attribute__((__packed__)) Vertex {
        float x, y, z;
        Color color;
        float u, v;
    };

    //TODO: Optimized Vertex structure - u16 x,y,z - u16 color - u16 u,v
    //TODO: Lit data structure including normals

    /**
     * @brief Mesh takes ownership of vertices and indices
     */
    class Mesh : public NonCopy {
    public:
        Mesh() = default;

        Mesh(Vertex* vertices, u16* indices, size_t idx_count);
        ~Mesh();

        auto add_data(Vertex* vertices, u16* indices, size_t idx_count) -> void;
        auto delete_data() -> void;

        auto draw() -> void;
        auto bind() -> void;

        inline auto get_index_count() -> s32 {
            return idx_count;
        }

    private:
        size_t idx_count = 0;
        Vertex* vert_data = nullptr;
        u16* idx_data = nullptr;
    };

}