#pragma once

#include <Rendering/Mesh.hpp>
#include <Utilities/Types.hpp>

namespace Stardust_Celeste::Graphics::G2D {

/**
 * @brief Definition of an individual tile object
 *
 */
struct Tile {
    Rendering::Rectangle bounds;
    Rendering::Color color;
    u16 index;
    float layer;
};

/**
 * @brief Tilemap class, which consists of a list of tiles to draw in batch from
 * a single atlas
 *
 */
class Tilemap {
  public:
    Tilemap(u32 texture, glm::vec2 atlasSize);
    virtual ~Tilemap();

    /**
     * @brief Adds a tile to the internal tile list
     *
     * @param tile Tile to add
     */
    virtual auto add_tile(Tile tile) -> void;

    /**
     * @brief Generates the map mesh -- must be called before draw
     *
     */
    virtual auto generate_map() -> void;

    /**
     * @brief Adds a list of tiles to insert into the internal tile list
     *
     * @param tiles Tile list to add
     */
    virtual auto add_tiles(std::vector<Tile> tiles) -> void;

    /**
     * @brief Clears the internal tile list
     *
     */
    virtual auto clear_tiles() -> void;

    /**
     * @brief Base method does nothing
     *
     * @param dt Delta Time
     */
    virtual auto update(double dt) -> void;

    /**
     * @brief Draws the map to the screen
     *
     */
    virtual auto draw() -> void;

    /**
     * @brief Texture ID
     *
     */
    u32 texture;

  protected:
    std::vector<Tile> tileMap;
    ScopePtr<Rendering::Mesh> mesh;
    std::vector<Rendering::Vertex> vert_data;
    std::vector<u16> idx_data;
    glm::vec2 atlasDimensions;
};

} // namespace Stardust_Celeste::Graphics::G2D