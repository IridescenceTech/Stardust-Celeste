#pragma once
#include "Tilemap.hpp"
#include <Rendering/Mesh.hpp>
#include <Utilities/Types.hpp>
#include <vector>
#include <map>

namespace Stardust_Celeste::Graphics::G2D
{

  struct TextData
  {
    std::string text;
    glm::vec2 pos;
    Rendering::Color color;
    float layer;
  };

  class FontRenderer : private Tilemap
  {
  public:
    FontRenderer(u32 texture, glm::vec2 atlasSize);
    ~FontRenderer();

    auto add_text(std::string text, glm::vec2 position, Rendering::Color color, float layer)
        -> void;
    auto clear() -> void;

    auto update(double dt) -> void;
    auto draw() -> void;

    float *size_map;
    float scale_factor;

  private:
    auto rebuild() -> void;

    std::vector<TextData> stringVector;
  };

} // namespace Stardust_Celeste::Graphics::G2D
