#pragma once
#include <Graphics/2D/Tilemap.hpp>

namespace Stardust_Celeste::Graphics::G2D {

/**
 * @brief AnimatedTile extends Tile and adds a start and finishing index per
 * each tile
 *
 */
struct AnimatedTile : public Tile {
    u16 start_idx, final_idx;
};

/**
 * @brief AnimatedTilemap
 *
 */
class AnimatedTilemap : public Tilemap {
  public:
    /**
     * @brief Construct a new Animated Tilemap object
     *
     * @param texture Texture ID
     * @param atlasSize Texture Atlas Size
     */
    AnimatedTilemap(u32 texture, Math::Vector2<float> atlasSize);
    virtual ~AnimatedTilemap();

    /**
     * @brief Adds animated tile to the internal tile set
     *
     * @param tile Animated Tile
     */
    virtual auto add_tile(AnimatedTile tile) -> void;

/**
 * @brief Adds a list of animated tiles to the internal tile set
 *
 * @param tile Animated Tile List to insert
 */
#if USE_EASTL
    virtual auto add_tiles(eastl::vector<AnimatedTile> tiles) -> void;
#else
    virtual auto add_tiles(std::vector<AnimatedTile> tiles) -> void;
#endif

    /**
     * @brief Clears tile set
     *
     */
    virtual auto clear_tiles() -> void override;

    /**
     * @brief Updates the tilemap to tick based on tickPerSec
     *
     * @param dt Delta Time
     */
    virtual auto update(double dt) -> void override;

    /**
     * @brief Generates the map mesh to draw
     *
     */
    virtual auto generate_map() -> void override;

    /**
     * @brief Ticks every tile to the next animation frame
     *
     */
    virtual auto tick() -> void;

    /**
     * @brief Number of ticks (animation frame updates) per second
     *
     */
    float ticksPerSec;

  protected:
    float atime;
    std::vector<AnimatedTile> atileMap;
};

} // namespace Stardust_Celeste::Graphics::G2D
