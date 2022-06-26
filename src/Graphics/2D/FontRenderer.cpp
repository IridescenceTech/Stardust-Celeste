#include <Graphics/2D/FontRenderer.hpp>
#include <Rendering/Texture.hpp>
#include <ext/stb_image.hpp>

namespace Stardust_Celeste::Graphics::G2D {

FontRenderer::FontRenderer(u32 texture, glm::vec2 atlasSize)
    : Tilemap(texture, atlasSize) {
    size_map = (float *)malloc(atlasSize.x * atlasSize.y * sizeof(float));
    scale_factor = 1.0f;

    for (int i = 0; i < atlasSize.x * atlasSize.y; i++)
        size_map[i] = 8;

    int width, height, nrChannels;
    Rendering::Color *data = (Rendering::Color *)stbi_load(
        Rendering::TextureManager::get().get_texture(texture)->name.c_str(),
        &width, &height, &nrChannels, STBI_rgb_alpha);

    int w_per_char = width / atlasSize.x;
    int h_per_char = height / atlasSize.y;

    for (int i = 0; i < atlasSize.x * atlasSize.y; i++) {
        int x = i % (int)atlasSize.x;
        int y = i / atlasSize.x;

        auto len_cal = 1;

        for (int sx = x * w_per_char; sx < (x + 1) * w_per_char; sx++) {
            bool hit = false;
            for (int sy = y * h_per_char; sy < (y + 1) * h_per_char; sy++) {
                auto idx = sx + sy * width;
                if (data[idx].rgba.a != 0)
                    hit = true;
            }

            if (hit)
                len_cal = sx - x * w_per_char;
        }

        size_map[i] = len_cal + 2;
    }

    stbi_image_free(data);
}

FontRenderer::~FontRenderer() { free(size_map); }

auto FontRenderer::add_text(std::string text, glm::vec2 position,
                            Rendering::Color color, float layer) -> void {
    stringVector.push_back({text, position, color, layer});
}

auto FontRenderer::clear() -> void {
    stringVector.clear();
    stringVector.shrink_to_fit();
}

auto FontRenderer::update(double dt) -> void { Tilemap::update(dt); }
auto FontRenderer::draw() -> void { Tilemap::draw(); }

auto FontRenderer::rebuild() -> void {

    clear_tiles();

    for (auto &s : stringVector) {
        auto pos = s.pos;
        for (int i = 0; i < s.text.length(); i++) {
            auto c = s.text[i];

            if (c < 0) {
                c = 0;
            } else if (c >= 128) {
                c = 128;
            }

            add_tile({{pos, glm::vec2(8 * scale_factor, 8 * scale_factor)},
                      s.color,
                      static_cast<u16>(c),
                      s.layer});
            pos.x += size_map[c] * scale_factor;
        }
    }

    generate_map();
}

} // namespace Stardust_Celeste::Graphics::G2D
