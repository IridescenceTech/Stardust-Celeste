#pragma once
#include "Tilemap.hpp"
#include <Rendering/Mesh.hpp>
#include <Utilities/Types.hpp>
#include <map>
#include <vector>

namespace Stardust_Celeste::Graphics::G2D {

struct TextData {
    std::string text;
    glm::vec2 pos;
    Rendering::Color color;
    float layer;
};

class FontRenderer : public Tilemap {
  public:
    FontRenderer(u32 texture, glm::vec2 atlasSize);
    ~FontRenderer();

    auto add_text(std::string text, glm::vec2 position, Rendering::Color color,
                  float layer) -> void;
    auto clear() -> void;

    auto update(double dt) -> void;
    auto draw() -> void;

    float *size_map;
    float scale_factor;

    auto rebuild() -> void;

    auto calculate_size(std::string text) -> float;

  private:
    std::vector<TextData> stringVector;
};

} // namespace Stardust_Celeste::Graphics::G2D
