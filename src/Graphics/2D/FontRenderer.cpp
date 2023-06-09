#include <Graphics/2D/FontRenderer.hpp>
#include <Rendering/Texture.hpp>
#include <cstdlib>
#include <stb_image.hpp>

namespace Stardust_Celeste::Graphics::G2D {

FontRenderer::FontRenderer(u32 texture, mathfu::Vector<float, 2> atlasSize)
    : Tilemap(texture, atlasSize) {

    SC_CORE_ASSERT(texture != 0, "Tilemap construction: Texture ID is 0!");
    SC_CORE_ASSERT(atlasSize.x * atlasSize.y > 0,
                   "Tilemap construction: Atlas Size is <= 0!");

    size_map = (float *)malloc(atlasSize.x * atlasSize.y * sizeof(float));
    scale_factor = 1.0f;

    for (int i = 0; i < atlasSize.x * atlasSize.y; i++)
        size_map[i] = 8;

    auto tex = Rendering::TextureManager::get().get_texture(texture);
    Rendering::Color *data = (Rendering::Color *)tex->pixData;

    int w_per_char = tex->width / atlasSize.x;
    int h_per_char = tex->height / atlasSize.y;

    for (int i = 0; i < atlasSize.x * atlasSize.y; i++) {
        int x = i % (int)atlasSize.x;
        int y = i / atlasSize.x;

        auto len_cal = 1;

        for (int sx = x * w_per_char; sx < (x + 1) * w_per_char; sx++) {
            bool hit = false;
            for (int sy = y * h_per_char; sy < (y + 1) * h_per_char; sy++) {
                auto idx = sx + sy * tex->width;
                if (data[idx].rgba.a != 0)
                    hit = true;
            }

            if (hit)
                len_cal = sx - x * w_per_char;
        }

        size_map[i] = len_cal + 2;
    }
}

FontRenderer::~FontRenderer() { free(size_map); }

auto FontRenderer::add_text(std::string text, mathfu::Vector<float, 2> position,
                            Rendering::Color color, float layer) -> void {
    stringVector.push_back({text, position, color, layer});
}

auto FontRenderer::clear_tiles() -> void {
    stringVector.clear();
    stringVector.shrink_to_fit();

    Tilemap::clear_tiles();
}

auto FontRenderer::generate_map() -> void {

    Tilemap::clear_tiles();

    for (auto &s : stringVector) {
        auto pos = s.pos;
        for (int i = 0; i < s.text.length(); i++) {
            auto c = s.text[i];

            if (c < 0) {
                c = 0;
            } else if (c >= 127) {
                c = 127;
            }

            add_tile({{pos, mathfu::Vector<float, 2>(8 * scale_factor, 8 * scale_factor)},
                      s.color,
                      static_cast<u16>(c),
                      s.layer});
            pos.x += size_map[c] * scale_factor;
        }
    }

    Tilemap::generate_map();
}

auto FontRenderer::calculate_size(std::string text) -> float {
    float x = 0;

    for (int i = 0; i < text.length(); i++) {
        auto c = text[i];
        x += size_map[c] * scale_factor;
    }

    return x;
}

} // namespace Stardust_Celeste::Graphics::G2D
