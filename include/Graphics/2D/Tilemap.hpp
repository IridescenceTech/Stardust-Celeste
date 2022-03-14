#pragma once

#include <Rendering/Mesh.hpp>
#include <Utilities/Types.hpp>

namespace Stardust_Celeste::Graphics::G2D {

struct Tile {
    Rendering::Rectangle bounds;
    Rendering::Color color;
    u16 index;
};

class Tilemap {
  public:
    Tilemap(u32 texture, glm::vec2 atlasSize);
    ~Tilemap();

    auto add_tile(Tile tile) -> void;
    auto add_tiles(std::vector<Tile> tiles) -> void;
    auto clear_tiles() -> void;

    auto update(double dt) -> void;
    auto draw() -> void;

  protected:
    auto generate_map() -> void;

  private:
    u32 texture;

    std::vector<Tile> tileMap;
    ScopePtr<Rendering::Mesh> mesh;
    std::vector<Rendering::Vertex> vert_data;
    std::vector<u16> idx_data;
    glm::vec2 atlasDimensions;
};

} // namespace Stardust_Celeste::Graphics::G2D