#pragma once
#include <Rendering/Mesh.hpp>
#include <Utilities/Types.hpp>

namespace Stardust_Celeste::Graphics::G2D {

    class Sprite {
        public:
            Sprite(u32 texture);

        private:
            Rendering::Rectangle bounds;
            u32 texture;
            Rendering::Mesh mesh;
    }

}