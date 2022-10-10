#include <Graphics/2D/AnimatedSprite.hpp>

namespace Stardust_Celeste::Graphics::G2D {
    AnimatedSprite::AnimatedSprite(u32 texture, Rendering::Rectangle bounds, glm::vec2 atlasSize) : Sprite(texture, bounds), tickTimer(0.0f) {
        startIDX = 0;
        endIDX = atlasSize.x * atlasSize.y;
        atlas = atlasSize;
        ticksPerSec = 4.0f;
        currentIDX = 0;
    }
    AnimatedSprite::~AnimatedSprite() {

    }

    auto AnimatedSprite::tick() -> void {
        currentIDX++;

        if(currentIDX > endIDX)
            currentIDX = startIDX;
        
        if(currentIDX < startIDX)
            currentIDX = startIDX;
        
        //UPDATE UVs
        auto uvs = Rendering::Texture::get_tile_uvs(atlas, currentIDX);

        verts[0].u = uvs[0];
        verts[0].v = uvs[1];
        verts[1].u = uvs[2];
        verts[1].v = uvs[3];
        verts[2].u = uvs[4];
        verts[2].v = uvs[5];
        verts[3].u = uvs[6];
        verts[3].v = uvs[7];

        mesh->add_data(verts, 4, idxs, 6);
    }

    auto AnimatedSprite::update(double dt) -> void {
        tickTimer += dt;

        if(tickTimer > 1.0f / ticksPerSec) {
            tickTimer = 0.0f;
            tick();
        }
    }

}