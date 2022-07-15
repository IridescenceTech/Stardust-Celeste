#pragma once
#include <Rendering/Mesh.hpp>
#include <Utilities/Types.hpp>

namespace Stardust_Celeste::Graphics::G2D {

class Sprite {
  public:
    Sprite(u32 texture, Rendering::Rectangle bounds);
    Sprite(u32 texture, Rendering::Rectangle bounds,
           Rendering::Rectangle selection);
    Sprite(u32 texture, Rendering::Rectangle bounds,
           Rendering::Rectangle selection, Rendering::Color color);
    Sprite(u32 texture, Rendering::Rectangle bounds, Rendering::Color color);
    ~Sprite();

    auto update(double dt) -> void;
    auto draw() -> void;

    auto set_position(glm::vec2 position) -> void;
    auto set_size(glm::vec2 position) -> void;
    auto set_rect(Rendering::Rectangle bounds) -> void;
    auto set_selection(Rendering::Rectangle bounds) -> void;
    auto set_layer(s16 layer) -> void;
    auto set_color(Rendering::Color color) -> void;

  protected:
    auto update_mesh() -> void;

  private:
    Rendering::Rectangle selection;
    Rendering::Rectangle bounds;
    Rendering::Color color;
    u32 texture;
    s16 layer;
    ScopePtr<Rendering::Mesh> mesh;
    Rendering::Vertex *verts;
    u16 *idxs;
};

} // namespace Stardust_Celeste::Graphics::G2D