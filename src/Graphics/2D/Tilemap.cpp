#include <Graphics/2D/Tilemap.hpp>
#include <Rendering/Texture.hpp>
#include <Utilities/Assertion.hpp>

namespace Stardust_Celeste::Graphics::G2D {

Tilemap::Tilemap(u32 tex, Math::Vector2<float> atlasSize) {
    SC_CORE_ASSERT(tex != 0, "Tilemap construction: Texture ID is 0!");
    SC_CORE_ASSERT(atlasSize.x * atlasSize.y > 0,
                   "Tilemap construction: Atlas Size is <= 0!");
    texture = tex;
    atlasDimensions = atlasSize;
    mesh = create_scopeptr<Rendering::Mesh<Rendering::Vertex>>();
}

Tilemap::~Tilemap() {
    mesh->delete_data();
    tileMap.clear();
}

auto Tilemap::add_tile(Tile tile) -> void { tileMap.push_back(tile); }

#if USE_EASTL
auto Tilemap::add_tiles(eastl::vector<Tile> tiles) -> void {
#else
auto Tilemap::add_tiles(std::vector<Tile> tiles) -> void {
#endif
    SC_CORE_ASSERT(tiles.size() > 0,
                   "Tilemap, tile array insertion is of size() <= 0");
    tileMap.insert(tileMap.end(), tiles.begin(), tiles.end());
}

auto Tilemap::clear_tiles() -> void {
    tileMap.clear();
    tileMap.shrink_to_fit();
    mesh->clear_data();
}

auto Tilemap::update(double dt) -> void {
    // Base class: Do nothing
}

auto Tilemap::draw() -> void {
    Rendering::TextureManager::get().bind_texture(texture);
    if (mesh != nullptr)
        mesh->draw();
}

auto Tilemap::generate_map() -> void {
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

#if PSP
    sceKernelDcacheWritebackInvalidateAll();
#endif // PSP
}

} // namespace Stardust_Celeste::Graphics::G2D