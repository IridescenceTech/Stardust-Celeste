#include <Graphics/2D/FixedTilemap.hpp>
#include <Rendering/Texture.hpp>
#include <Utilities/Assertion.hpp>

namespace Stardust_Celeste::Graphics::G2D {

template <size_t N>
FixedTilemap<N>::FixedTilemap(u32 tex, glm::vec2 atlasSize) {
    SC_CORE_ASSERT(tex != 0, "FixedTilemap construction: Texture ID is 0!");
    SC_CORE_ASSERT(atlasSize.x * atlasSize.y > 0,
                   "FixedTilemap construction: Atlas Size is <= 0!");
    texture = tex;
    atlasDimensions = atlasSize;
    mesh = create_scopeptr<Rendering::Mesh<Rendering::Vertex>>();
}

template <size_t N> FixedTilemap<N>::~FixedTilemap() {
    mesh->delete_data();
    tileMap.clear();
}

template <size_t N> auto FixedTilemap<N>::add_tile(Tile tile) -> void {
    tileMap.push_back(tile);
}

template <size_t N>
#if USE_EASTL
auto FixedTilemap<N>::add_tiles(eastl::vector<Tile> tiles) -> void {
#else
auto FixedTilemap<N>::add_tiles(std::vector<Tile> tiles) -> void {
#endif
    SC_CORE_ASSERT(tiles.size() > 0,
                   "FixedTilemap, tile array insertion is of size() <= 0");
    tileMap.insert(tileMap.end(), tiles.begin(), tiles.end());
}

template <size_t N> auto FixedTilemap<N>::clear_tiles() -> void {
    tileMap.clear();
    tileMap.shrink_to_fit();
}

template <size_t N> auto FixedTilemap<N>::update(double dt) -> void {
    // Base class: Do nothing
}

template <size_t N> auto FixedTilemap<N>::draw() -> void {
    Rendering::TextureManager::get().bind_texture(texture);
    if (mesh != nullptr)
        mesh->draw();
}

template <size_t N> auto FixedTilemap<N>::generate_map() -> void {
    mesh->delete_data();

    auto idxc = 0;

    for (auto &t : tileMap) {
        auto x = t.bounds.position.x;
        auto y = t.bounds.position.y;
        auto w = t.bounds.extent.x;
        auto h = t.bounds.extent.y;

        auto uvs = Rendering::Texture::get_tile_uvs(atlasDimensions, t.index);

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

        mesh->vertices.push_back(
            Rendering::Vertex{uvs[0], uvs[1], t.color, x, y, t.layer});
        mesh->vertices.push_back(
            Rendering::Vertex{uvs[2], uvs[3], t.color, x + w, y, t.layer});
        mesh->vertices.push_back(
            Rendering::Vertex{uvs[4], uvs[5], t.color, x + w, y + h, t.layer});
        mesh->vertices.push_back(
            Rendering::Vertex{uvs[6], uvs[7], t.color, x, y + h, t.layer});

        mesh->indices.insert(mesh->indices.end(),
                             {(u16)(idxc + 0), (u16)(idxc + 1), (u16)(idxc + 2),
                              (u16)(idxc + 2), (u16)(idxc + 3),
                              (u16)(idxc + 0)});
        idxc += 4;
    }

    mesh->setup_buffer();
}

} // namespace Stardust_Celeste::Graphics::G2D