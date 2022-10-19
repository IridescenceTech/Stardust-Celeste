#pragma once
#include <Graphics/2D/Sprite.hpp>

namespace Stardust_Celeste::Graphics::G2D {

class AnimatedSprite : public Sprite {
    public:
    AnimatedSprite(u32 texture, Rendering::Rectangle bounds, glm::vec2 atlasSize);
    virtual ~AnimatedSprite();

    auto tick() -> void;
    auto update(double dt) -> void override;

    inline auto set_animation_range(u32 startIDX, u32 endIDX) -> void {
        this->startIDX = startIDX;
        this->endIDX = endIDX;
    }

    float ticksPerSec;
    private:
    float tickTimer;
    glm::vec2 atlas;
    u32 startIDX, endIDX, currentIDX;
};

}