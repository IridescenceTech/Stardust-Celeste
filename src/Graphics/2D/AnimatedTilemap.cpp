#include <Graphics/2D/AnimatedTilemap.hpp>
#include <Rendering/Texture.hpp>

namespace Stardust_Celeste::Graphics::G2D {

AnimatedTilemap::AnimatedTilemap(u32 tex, glm::vec2 atlasSize) : Tilemap(tex, atlasSize) {
    atileMap.clear();
    atime = 0.0f;
    tickPerSec = 4.0f;
}

AnimatedTilemap::~AnimatedTilemap() {
    atileMap.clear();
}

auto AnimatedTilemap::add_tile(AnimatedTile tile) -> void { atileMap.push_back(tile); }

auto AnimatedTilemap::add_tiles(std::vector<AnimatedTile> tiles) -> void {
    atileMap.insert(atileMap.end(), tiles.begin(), tiles.end());
}

auto AnimatedTilemap::clear_tiles() -> void {
    atileMap.clear();
    atileMap.shrink_to_fit();
}

auto AnimatedTilemap::update(double dt) -> void {
    atime += dt;

    if(atime > 1.0f / tickPerSec) {
        tick();
        atime = 0.0f;
    }
}


auto AnimatedTilemap::tick() -> void {
    for(auto& t : atileMap) {
        t.index++;

        if(t.index < t.start_idx || t.index >= t.final_idx) {
            t.index = t.start_idx;
        }
    }

    generate_map();
}

auto AnimatedTilemap::generate_map() -> void {
    mesh->delete_data();

    vert_data.clear();
    vert_data.shrink_to_fit();
    idx_data.clear();
    idx_data.shrink_to_fit();

    auto idxc = 0;

    for (auto &t : atileMap) {
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

        vert_data.push_back(
            Rendering::Vertex{uvs[0], uvs[1], t.color, x, y, t.layer});
        vert_data.push_back(
            Rendering::Vertex{uvs[2], uvs[3], t.color, x + w, y, t.layer});
        vert_data.push_back(
            Rendering::Vertex{uvs[4], uvs[5], t.color, x + w, y + h, t.layer});
        vert_data.push_back(
            Rendering::Vertex{uvs[6], uvs[7], t.color, x, y + h, t.layer});

        idx_data.insert(idx_data.end(),
                        {(u16)(idxc + 0), (u16)(idxc + 1), (u16)(idxc + 2),
                         (u16)(idxc + 2), (u16)(idxc + 3), (u16)(idxc + 0)});
        idxc += 4;
    }

    mesh->add_data(vert_data.data(), vert_data.size(), idx_data.data(),
                   idx_data.size());
}

} // namespace Stardust_Celeste::Graphics::G2D