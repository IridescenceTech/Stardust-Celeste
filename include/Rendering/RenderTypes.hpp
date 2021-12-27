#pragma once
#include <Utilities/Types.hpp>

namespace Stardust_Celeste::Rendering
{
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

}