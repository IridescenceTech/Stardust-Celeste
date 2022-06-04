#include <Graphics/2D/Tilemap.hpp>
#include <Rendering/Texture.hpp>

namespace Stardust_Celeste::Graphics::G2D {

Tilemap::Tilemap(u32 tex, glm::vec2 atlasSize) {
    texture = tex;
    atlasDimensions = atlasSize;
}

Tilemap::~Tilemap() {
    vert_data.clear();
    idx_data.clear();
    tileMap.clear();
}

auto Tilemap::add_tile(Tile tile) -> void { tileMap.push_back(tile); }

auto Tilemap::add_tiles(std::vector<Tile> tiles) -> void {
    tileMap.insert(tileMap.end(), tiles.begin(), tiles.end());
}

auto Tilemap::clear_tiles() -> void { tileMap.clear(); }

auto Tilemap::update(double dt) -> void {
    // Base class: Do nothing
}

auto Tilemap::draw() -> void {
    Rendering::TextureManager::get().bind_texture(texture);
    if (mesh != nullptr)
        mesh->draw();
}

auto Tilemap::generate_map() -> void {

    vert_data.clear();
    vert_data.shrink_to_fit();
    idx_data.clear();
    idx_data.shrink_to_fit();

    auto idxc = 0;

    for (auto& t : tileMap) {
        auto x = t.bounds.position.x;
        auto y = t.bounds.position.y;
        auto w = t.bounds.extent.x;
        auto h = t.bounds.extent.y;

        auto uvs = Rendering::Texture::get_tile_uvs(atlasDimensions, t.index);

        vert_data.push_back(
            Rendering::Vertex{uvs[0], uvs[1], t.color, x, y, 0});
        vert_data.push_back(
            Rendering::Vertex{uvs[2], uvs[3], t.color, x + w, y, 0});
        vert_data.push_back(
            Rendering::Vertex{uvs[4], uvs[5], t.color, x + w, y + h, 0});
        vert_data.push_back(
            Rendering::Vertex{uvs[6], uvs[7], t.color, x, y + h, 0});

        idx_data.insert(idx_data.end(),
                        {(u16)(idxc + 0), (u16)(idxc + 1), (u16)(idxc + 2),
                         (u16)(idxc + 2), (u16)(idxc + 3), (u16)(idxc + 0)});
        idxc += 4;
    }

    if (mesh.get() == nullptr) {
        mesh =
            create_scopeptr<Rendering::Mesh>(vert_data.data(), vert_data.size(),
                                             idx_data.data(), idx_data.size());
    } else {
        mesh->delete_data();
        mesh->add_data(vert_data.data(), vert_data.size(), idx_data.data(),
                       idx_data.size());
    }
}

} // namespace Stardust_Celeste::Graphics::G2D