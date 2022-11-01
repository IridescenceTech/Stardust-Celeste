#pragma once

#include <Graphics/2D/Tilemap.hpp>
#include <Rendering/Mesh.hpp>
#include <Utilities/Types.hpp>

namespace Stardust_Celeste::Graphics::G2D {

/**
 * @brief FixedTilemap class, which consists of a fixed size list of tiles to
 * draw in batch from a single atlas.
 *
 */
template <size_t N> class FixedTilemap {
  public:
    FixedTilemap(u32 texture, glm::vec2 atlasSize);
    virtual ~FixedTilemap();

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
    virtual auto generate_map() -> void override;

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
    std::array<Tile, N> tileMap;
    ScopePtr<Rendering::FixedMesh<Rendering::Vertex, 4 * N, 6 * N>> mesh;
    glm::vec2 atlasDimensions;
};

} // namespace Stardust_Celeste::Graphics::G2D