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

    auto w = 1.0f / atlas.x;
    auto h = 1.0f / atlas.y;

    auto idx = currentIDX % static_cast<int>(atlas.x);
    auto idy = currentIDX / static_cast<int>(atlas.x);

    Rendering::Rectangle selection = { {idx * w, idy * h}, {w, h}};
    set_selection(selection);
}

auto AnimatedSprite::update(double dt) -> void {
    tickTimer += dt;

    if (tickTimer > 1.0f / ticksPerSec) {
        tickTimer = 0.0f;
        tick();
    }
}

} // namespace Stardust_Celeste::Graphics::G2D