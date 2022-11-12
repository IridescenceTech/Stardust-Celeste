#pragma once
#include <Graphics/2D/Tilemap.hpp>
#include <Rendering/Mesh.hpp>
#include <Rendering/Texture.hpp>
#include <Utilities/Types.hpp>

namespace Stardust_Celeste::Graphics::G2D {

/**
 * @brief FixedTilemap class, which consists of a fixed size list of tiles to
 * draw in batch from a single atlas.
 *
 */
template <size_t N> class FixedTilemap {
  public:
    FixedTilemap(u32 tex, glm::vec2 atlasSize) {
        SC_CORE_ASSERT(tex != 0, "FixedTilemap construction: Texture ID is 0!");
        SC_CORE_ASSERT(atlasSize.x * atlasSize.y > 0,
                       "FixedTilemap construction: Atlas Size is <= 0!");
        texture = tex;
        atlasDimensions = atlasSize;
        mesh = create_scopeptr<
            Rendering::FixedMesh<Rendering::Vertex, N * 4, N * 6>>();
    }
    virtual ~FixedTilemap() {
        if (mesh != nullptr)
            mesh->delete_data();
    }

    /**
     * @brief Adds a tile to the internal tile list
     *
     * @param tile Tile to add
     */
    virtual auto add_tile(Tile tile) -> void { tileMap[count++] = tile; }

    /**
     * @brief Generates the map mesh -- must be called before draw
     *
     */
    virtual auto generate_map() -> void {

        mesh->clear_data();

        auto idxc2 = 0;
        auto idxc = 0;

        for (auto &t : tileMap) {
            auto x = t.bounds.position.x;
            auto y = t.bounds.position.y;
            auto w = t.bounds.extent.x;
            auto h = t.bounds.extent.y;

            auto uvs =
                Rendering::Texture::get_tile_uvs(atlasDimensions, t.index);

#if PSP
            auto tInfo = Rendering::TextureManager::get().get_texture(texture);

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

            mesh->vertices[idxc + 0] =
                Rendering::Vertex{uvs[0], uvs[1], t.color, x, y, t.layer};
            mesh->vertices[idxc + 1] =
                Rendering::Vertex{uvs[2], uvs[3], t.color, x + w, y, t.layer};
            mesh->vertices[idxc + 2] = Rendering::Vertex{
                uvs[4], uvs[5], t.color, x + w, y + h, t.layer};
            mesh->vertices[idxc + 3] =
                Rendering::Vertex{uvs[6], uvs[7], t.color, x, y + h, t.layer};

            mesh->indices[idxc2 + 0] = idxc + 0;
            mesh->indices[idxc2 + 1] = idxc + 1;
            mesh->indices[idxc2 + 2] = idxc + 2;
            mesh->indices[idxc2 + 3] = idxc + 2;
            mesh->indices[idxc2 + 4] = idxc + 3;
            mesh->indices[idxc2 + 5] = idxc + 0;

            idxc += 4;
            idxc2 += 6;
        }

        mesh->setup_buffer();
    }

    /**
     * @brief Clears the internal tile list
     *
     */
    virtual auto clear_tiles() -> void {
        if (tileMap.empty())
            return;

        for (int i = 0; i < N; i++) {
            tileMap[i].bounds = {{0, 0}, {0, 0}};
            tileMap[i].color.color = 0;
            tileMap[i].index = 0;
            tileMap[i].layer = 0;
        }
        count = 0;
    }

    /**
     * @brief Base method does nothing
     *
     * @param dt Delta Time
     */
    virtual auto update(double dt) -> void {}

    /**
     * @brief Draws the map to the screen
     *
     */
    virtual auto draw() -> void {
        Rendering::TextureManager::get().bind_texture(texture);
        if (mesh != nullptr)
            mesh->draw();
    }

    /**
     * @brief Texture ID
     *
     */
    u32 texture;

  protected:
    int count;
    std::array<Tile, N> tileMap;
    ScopePtr<Rendering::FixedMesh<Rendering::Vertex, 4 * N, 6 * N>> mesh;
    glm::vec2 atlasDimensions;
};

} // namespace Stardust_Celeste::Graphics::G2D