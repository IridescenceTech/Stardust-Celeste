#include <Graphics/2D/FontRenderer.hpp>
#include <Rendering/Texture.hpp>

namespace Stardust_Celeste::Graphics::G2D {

FontRenderer::FontRenderer(u32 texture, glm::vec2 atlasSize)
    : Tilemap(texture, atlasSize) {}

FontRenderer::~FontRenderer() {}

auto FontRenderer::add_text(std::string text, glm::vec2 position) -> void {
    stringMap.emplace(text, position);
    rebuild();
}

auto FontRenderer::clear() -> void {
    stringMap.clear();
    rebuild();
}

auto FontRenderer::update(double dt) -> void { Tilemap::update(dt); }
auto FontRenderer::draw() -> void { Tilemap::draw(); }

auto FontRenderer::rebuild() -> void {

    clear_tiles();

    for (auto const &[key, val] : stringMap) {
        auto pos = val;
        for (int i = 0; i < key.length(); i++) {
            auto c = key[i];

            if (c < 0) {
                c = 0;
            } else if (c >= 128) {
                c = 128;
            }

            add_tile({{pos, glm::vec2(8, 8)},
                      {255, 255, 255, 255},
                      static_cast<u16>(c)});
            pos.x += 8;
        }
    }

    generate_map();
}

} // namespace Stardust_Celeste::Graphics::G2D