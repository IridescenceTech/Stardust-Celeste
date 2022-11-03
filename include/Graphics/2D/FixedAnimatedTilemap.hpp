#pragma once
#include <Graphics/2D/FixedTilemap.hpp>
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
 * @brief FixedAnimatedTilemap
 *
 */
template <size_t N> class FixedAnimatedTilemap : public FixedTilemap<N> {
  public:
    /**
     * @brief Construct a new Animated Tilemap object
     *
     * @param texture Texture ID
     * @param atlasSize Texture Atlas Size
     */
    FixedAnimatedTilemap(u32 texture, glm::vec2 atlasSize): FixedTilemap<N>(tex, atlasSize) {
        atileMap.clear();
        atime = 0.0f;
        ticksPerSec = 4.0f;
    }
    
    virtual ~FixedAnimatedTilemap() {
        atileMap.clear();
    }

    /**
     * @brief Adds animated tile to the internal tile set
     *
     * @param tile Animated Tile
     */
    virtual auto add_tile(AnimatedTile tile) -> void {
        atileMap[count++] = tile;
    }

    /**
     * @brief Clears tile set
     *
     */
    virtual auto clear_tiles() -> void override {
        FixedTilemap<N>::clear_tiles();
    }

    /**
     * @brief Updates the tilemap to tick based on tickPerSec
     *
     * @param dt Delta Time
     */
    virtual auto update(double dt) -> void override {
        atime += dt;

        if (atime > 1.0f / ticksPerSec) {
            tick();
            atime = 0.0f;
        }
    }

    /**
     * @brief Generates the map mesh to draw
     *
     */
    virtual auto generate_map() -> void override {
    FixedTilemap<N>::mesh->delete_data();

    auto idxc = 0;

    for (auto &t : atileMap) {
        auto x = t.bounds.position.x;
        auto y = t.bounds.position.y;
        auto w = t.bounds.extent.x;
        auto h = t.bounds.extent.y;

        auto uvs = Rendering::Texture::get_tile_uvs(
            FixedTilemap<N>::atlasDimensions, t.index);

#if PSP
        auto tInfo = Rendering::TextureManager::get().get_texture(
            FixedTilemap<N>::texture);

        float wRatio = 1.0f;
        float hRatio = 1.0f;

        if (tInfo != nullptr) {
            wRatio = (float)tInfo->width / (float)tInfo->pW;
            hRatio = (float)tInfo->height / (float)tInfo->pH;
        }

        for (int i = 0; i < 4; i++) {
            uvs[i * 2 + 0] *= wRatio;
            uvs[i * 2 + 1] *= hRatio;
        }
#endif

        FixedTilemap<N>::mesh->vertices.push_back(
            Rendering::Vertex{uvs[0], uvs[1], t.color, x, y, t.layer});
        FixedTilemap<N>::mesh->vertices.push_back(
            Rendering::Vertex{uvs[2], uvs[3], t.color, x + w, y, t.layer});
        FixedTilemap<N>::mesh->vertices.push_back(
            Rendering::Vertex{uvs[4], uvs[5], t.color, x + w, y + h, t.layer});
        FixedTilemap<N>::mesh->vertices.push_back(
            Rendering::Vertex{uvs[6], uvs[7], t.color, x, y + h, t.layer});

        FixedTilemap<N>::mesh->indices.insert(
            FixedTilemap<N>::mesh->indices.end(),
            {(u16)(idxc + 0), (u16)(idxc + 1), (u16)(idxc + 2), (u16)(idxc + 2),
             (u16)(idxc + 3), (u16)(idxc + 0)});
        idxc += 4;
    }

    FixedTilemap<N>::mesh->setup_buffer();
    }

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
    std::array<AnimatedTile, N> atileMap;
};

} // namespace Stardust_Celeste::Graphics::G2D
