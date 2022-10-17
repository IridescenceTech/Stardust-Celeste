#pragma once
#include <Graphics/2D/Tilemap.hpp>

namespace Stardust_Celeste::Graphics::G2D {

/**
 * @brief AnimatedTile extends Tile and adds a start and finishing index per each tile
 * 
 */
struct AnimatedTile : public Tile{
    u16 start_idx, final_idx;
};

/**
 * @brief AnimatedTilemap 
 * 
 */
class AnimatedTilemap : public Tilemap {

    //TODO: Redo the virtuals for the base and this -- seems like we've duplicated code somehow
  public:
    /**
     * @brief Construct a new Animated Tilemap object
     * 
     * @param texture Texture ID
     * @param atlasSize Texture Atlas Size
     */
    AnimatedTilemap(u32 texture, glm::vec2 atlasSize);
    virtual ~AnimatedTilemap();

    /**
     * @brief Adds animated tile to the internal tile set
     * 
     * @param tile Animated Tile
     */
    auto add_tile(AnimatedTile tile) -> void;
    
    /**
     * @brief Adds a list of animated tiles to the internal tile set
     * 
     * @param tile Animated Tile List to insert
     */
    auto add_tiles(std::vector<AnimatedTile> tiles) -> void;

    /**
     * @brief Clears tile set
     * 
     */
    auto clear_tiles() -> void;

    //TODO: Naming consistency with AnimatedSprite?
    /**
     * @brief Updates the tilemap to tick based on tickPerSec
     * 
     * @param dt Delta Time
     */
    auto update(double dt) -> void override;

    /**
     * @brief Ticks every tile to the next animation frame
     * 
     */
    auto tick() -> void;

    /**
     * @brief Generates the map mesh to draw
     * 
     */
    auto generate_map() -> void;

    /**
     * @brief Number of ticks (animation frame updates) per second
     * 
     */
    float tickPerSec;

    protected:
    float atime;
    std::vector<AnimatedTile> atileMap;
};

} // namespace Stardust_Celeste::Graphics::G2D
