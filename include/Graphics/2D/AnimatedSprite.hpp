#pragma once
#include <Graphics/2D/Sprite.hpp>

namespace Stardust_Celeste::Graphics::G2D {

/**
 * @brief Animated Sprite
 *
 */
class AnimatedSprite : public Sprite {
  public:
    /**
     * @brief Construct a new Animated Sprite object
     *
     * @param texture Texture ID
     * @param bounds Bounding Rectangle of the Sprite
     * @param atlasSize Atlas Size
     */
    AnimatedSprite(u32 texture, Rendering::Rectangle bounds,
                   Math::Vector2<float> atlasSize);
    virtual ~AnimatedSprite();

    /**
     * @brief Tick updates to the next frame of an animation
     *
     */
    virtual auto tick() -> void;

    /**
     * @brief Update the animation frame based on ticksPerSec
     *
     * @param dt Delta Time
     */
    auto update(double dt) -> void override;

    /**
     * @brief Set the animation range of the atlas
     *
     * @param startIDX Starting Index
     * @param endIDX Ending Index
     */
    inline auto set_animation_range(u32 startIDX, u32 endIDX) -> void {
        this->startIDX = startIDX;
        this->endIDX = endIDX;
    }

    /**
     * @brief Ticks per second (default = 4)
     *
     */
    float ticksPerSec;

  protected:
    float tickTimer;
    Math::Vector2<float> atlas;
    u32 startIDX, endIDX, currentIDX;
};

} // namespace Stardust_Celeste::Graphics::G2D