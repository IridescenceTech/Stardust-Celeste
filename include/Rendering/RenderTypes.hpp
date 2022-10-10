#pragma once
#include <Utilities/Types.hpp>
#include <array>
#include <glm.hpp>
#include <string>

namespace Stardust_Celeste::Rendering {
union Color {
    struct RGBA {
        u8 r;
        u8 g;
        u8 b;
        u8 a;
    };

    RGBA rgba;
    u32 color;
};

struct Rectangle {
    glm::vec2 position;
    glm::vec2 extent;
};

struct Texture {
    u32 width, height;

    u16 *data;

    u32 minFilter, magFilter;
    bool repeating;

    // Identity for texture management system OR OpenGL
    u32 id;

    // Internal power-of-2 width / height
    u32 pW, pH;

    // Internal properties
    u32 ramSpace, colorMode, swizzle;

    /**
     * @brief Returns texture coordinates for a tile-based spritesheet given a
     * size and index
     * @param dimensions - The dimensions of the spritesheet
     * @param idx - The index to search for. Derived from x + y * width
     * @return Returns the 4 indices in counter-clockwise winding
     */
    inline static std::array<float, 8> get_tile_uvs(glm::vec2 dimensions,
                                                    int idx) {
        int row = idx / (int)dimensions.x;
        int column = idx % (int)dimensions.x;

        float sizeX = 1.f / ((float)dimensions.x);
        float sizeY = 1.f / ((float)dimensions.y);
        float y = (float)row * sizeY;
        float x = (float)column * sizeX;
        float h = y + sizeY;
        float w = x + sizeX;

        return {x, h, w, h, w, y, x, y};
    }

    std::string name;
};

} // namespace Stardust_Celeste::Rendering