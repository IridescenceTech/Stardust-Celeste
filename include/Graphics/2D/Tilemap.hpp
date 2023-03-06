#pragma once

#include <Rendering/Mesh.hpp>
#include <Utilities/Types.hpp>

#if USE_EASTL
#include <EASTL/map.h>
#include <EASTL/vector.h>
#endif

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
    Tilemap(u32 texture, Math::Vector2<float> atlasSize);
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

#if USE_EASTL
    virtual auto add_tiles(eastl::vector<Tile> tiles) -> void;
#else
    virtual auto add_tiles(std::vector<Tile> tiles) -> void;
#endif

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
#if USE_EASTL
    eastl::vector<Tile> tileMap;
#else
    std::vector<Tile> tileMap;
#endif
    ScopePtr<Rendering::Mesh<Rendering::Vertex>> mesh;
    Math::Vector2<float> atlasDimensions;
};

} // namespace Stardust_Celeste::Graphics::G2D