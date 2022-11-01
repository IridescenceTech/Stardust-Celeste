#include <Graphics/2D/FixedAnimatedTilemap.hpp>
#include <Rendering/Texture.hpp>

namespace Stardust_Celeste::Graphics::G2D {

template <size_t N>
FixedAnimatedTilemap<N>::FixedAnimatedTilemap(u32 tex, glm::vec2 atlasSize)
    : Tilemap(tex, atlasSize) {
    atileMap.clear();
    atime = 0.0f;
    ticksPerSec = 4.0f;
}

template <size_t N> FixedAnimatedTilemap<N>::~FixedAnimatedTilemap() {
    atileMap.clear();
}

template <size_t N>
auto FixedAnimatedTilemap<N>::add_tile(AnimatedTile tile) -> void {
    atileMap.push_back(tile);
}

template <size_t N>
auto FixedAnimatedTilemap<N>::add_tiles(std::vector<AnimatedTile> tiles)
    -> void {
    atileMap.insert(atileMap.end(), tiles.begin(), tiles.end());
}

template <size_t N> auto FixedAnimatedTilemap<N>::clear_tiles() -> void {
    atileMap.clear();
    atileMap.shrink_to_fit();

    FixedTilemap<N>::clear_tiles();
}

template <size_t N> auto FixedAnimatedTilemap<N>::update(double dt) -> void {
    atime += dt;

    if (atime > 1.0f / ticksPerSec) {
        tick();
        atime = 0.0f;
    }
}

template <size_t N> auto FixedAnimatedTilemap<N>::tick() -> void {
    for (auto &t : atileMap) {
        t.index++;

        if (t.index < t.start_idx || t.index >= t.final_idx) {
            t.index = t.start_idx;
        }
    }

    generate_map();
}

template <size_t N> auto FixedAnimatedTilemap<N>::generate_map() -> void {
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

} // namespace Stardust_Celeste::Graphics::G2D