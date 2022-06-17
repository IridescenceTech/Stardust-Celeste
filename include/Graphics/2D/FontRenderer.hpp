#pragma once
#include "Tilemap.hpp"
#include <Rendering/Mesh.hpp>
#include <Utilities/Types.hpp>
#include <map>

namespace Stardust_Celeste::Graphics::G2D {

class FontRenderer : private Tilemap {
  public:
    FontRenderer(u32 texture, glm::vec2 atlasSize);
    ~FontRenderer();

    auto add_text(std::string text, glm::vec2 position) -> void;
    auto clear() -> void;

    auto update(double dt) -> void;
    auto draw() -> void;

    float *size_map;

  private:
    auto rebuild() -> void;
    std::map<std::string, glm::vec2> stringMap;
};

} // namespace Stardust_Celeste::Graphics::G2D