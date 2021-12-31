#pragma once
#include <Utilities/Types.hpp>

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
};

} // namespace Stardust_Celeste::Rendering