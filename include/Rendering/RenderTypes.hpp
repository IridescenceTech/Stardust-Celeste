#pragma once
#include <Utilities/Types.hpp>
#include <array>
#include <Math/Math.hpp>
#include <string>

namespace Stardust_Celeste::Rendering {

    enum PrimType { PRIM_TYPE_TRIANGLE, PRIM_TYPE_LINE };

/**
 * @brief Color union object
 *
 */
union Color {
    struct RGBA {
        u8 r;
#if BUILD_PLAT == BUILD_3DS
        u8 b;
        u8 g;
#else
        u8 g;
        u8 b;
#endif
        u8 a;
    };

    RGBA rgba;
    u32 color;
};

#if PSP
#define VERT_PACKED __attribute__((__packed__))
#else
#define VERT_PACKED
#endif

/**
 * @brief Packed vertices
 */
struct VERT_PACKED Vertex {
    float u, v;
    Color color;
    float x, y, z;
};

/**
 * @brief Rectangle Structure
 * position -- Position of rectangle
 * extent -- Size of rectangle
 */
struct Rectangle {
    Math::Vector2<float> position;
    Math::Vector2<float> extent;
};

/**
 * @brief Texture Data
 *
 */
struct Texture {
    u32 width, height;

    void *data;

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
    inline static std::array<float, 8> get_tile_uvs(Math::Vector2<float> dimensions,
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

inline auto deg2rad(float deg) -> float { return deg / 180.0f * 3.14159f; }
inline auto rad2deg(float rad) -> float { return rad * 180.0f / 3.14159f; }

} // namespace Stardust_Celeste::Rendering