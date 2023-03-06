#include <Graphics/2D/AnimatedSprite.hpp>
#include <Rendering/Texture.hpp>

namespace Stardust_Celeste::Graphics::G2D {
AnimatedSprite::AnimatedSprite(u32 texture, Rendering::Rectangle bounds,
                               Math::Vector2<float> atlasSize)
    : Sprite(texture, bounds), tickTimer(0.0f) {
    startIDX = 0;
    endIDX = atlasSize.x * atlasSize.y;
    atlas = atlasSize;
    ticksPerSec = 4.0f;
    currentIDX = 0;
}
AnimatedSprite::~AnimatedSprite() {}

auto AnimatedSprite::tick() -> void {
    currentIDX++;

    if (currentIDX > endIDX)
        currentIDX = startIDX;

    if (currentIDX < startIDX)
        currentIDX = startIDX;

    // UPDATE UVs
    auto uvs = Rendering::Texture::get_tile_uvs(atlas, currentIDX);

    mesh->vertices[0].u = uvs[0];
    mesh->vertices[0].v = uvs[1];
    mesh->vertices[1].u = uvs[2];
    mesh->vertices[1].v = uvs[3];
    mesh->vertices[2].u = uvs[4];
    mesh->vertices[2].v = uvs[5];
    mesh->vertices[3].u = uvs[6];
    mesh->vertices[3].v = uvs[7];

#if PSP
    auto tInfo = Rendering::TextureManager::get().get_texture(texture);

    float wRatio = 1.0f;
    float hRatio = 1.0f;

    if (tInfo != nullptr) {
        wRatio = (float)tInfo->width / (float)tInfo->pW;
        hRatio = (float)tInfo->height / (float)tInfo->pH;
    }

    for (int i = 0; i < 4; i++) {
        mesh->vertices[i].u *= wRatio;
        mesh->vertices[i].v *= hRatio;
    }
#endif

    mesh->setup_buffer();
}

auto AnimatedSprite::update(double dt) -> void {
    tickTimer += dt;

    if (tickTimer > 1.0f / ticksPerSec) {
        tickTimer = 0.0f;
        tick();
    }
}

} // namespace Stardust_Celeste::Graphics::G2D