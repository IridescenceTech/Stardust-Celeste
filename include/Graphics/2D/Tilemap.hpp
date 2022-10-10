#pragma once

#include <Rendering/Mesh.hpp>
#include <Utilities/Types.hpp>

namespace Stardust_Celeste::Graphics::G2D
{

  struct Tile
  {
    Rendering::Rectangle bounds;
    Rendering::Color color;
    u16 index;
    float layer;
  };

  class Tilemap
  {
  public:
    Tilemap(u32 texture, glm::vec2 atlasSize);
    virtual ~Tilemap();

    auto add_tile(Tile tile) -> void;
    auto generate_map() -> void;
    auto add_tiles(std::vector<Tile> tiles) -> void;
    auto clear_tiles() -> void;

    virtual auto update(double dt) -> void;
    auto draw() -> void;

    u32 texture;
  protected:
    std::vector<Tile> tileMap;
    ScopePtr<Rendering::Mesh> mesh;
    std::vector<Rendering::Vertex> vert_data;
    std::vector<u16> idx_data;
    glm::vec2 atlasDimensions;
  };

} // namespace Stardust_Celeste::Graphics::G2D