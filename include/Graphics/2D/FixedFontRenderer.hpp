#pragma once
#include <Graphics/2D/FixedTilemap.hpp>
#include <Graphics/2D/FontRenderer.hpp>
#include <Rendering/Mesh.hpp>
#include <Rendering/Texture.hpp>
#include <Utilities/Types.hpp>
#include <map>
#include <stb_image.hpp>
#include <vector>

namespace Stardust_Celeste::Graphics::G2D {

/**
 * @brief Font Rendering Tilemap
 *
 */
template <size_t N> class FixedFontRenderer : public FixedTilemap<N> {
  public:
    /**
     * @brief Construct a new Font Renderer object
     *
     * @param texture Texture ID
     * @param atlasSize Texture Atlas Size
     */
    FixedFontRenderer(u32 texture, mathfu::Vector<float, 2> atlasSize)
        : FixedTilemap<N>(texture, atlasSize) {

        SC_CORE_ASSERT(texture != 0, "Tilemap construction: Texture ID is 0!");
        SC_CORE_ASSERT(atlasSize.x * atlasSize.y > 0,
                       "Tilemap construction: Atlas Size is <= 0!");

        auto size = atlasSize.x * atlasSize.y;
        size_map = (float *)malloc(size * sizeof(float));
        SC_CORE_ASSERT(size_map != nullptr, "Sizemap returned Null!");
        SC_CORE_INFO("{}", size);
        scale_factor = 1.0f;

        for (int i = 0; i < atlasSize.x * atlasSize.y; i++)
            size_map[i] = 8;

        Rendering::Texture* tex = Rendering::TextureManager::get().get_texture(texture);
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

        stbi_image_free(data);
    }

    /**
     * @brief Destroy the Font Renderer object
     *
     */
    virtual ~FixedFontRenderer() { free(size_map); }

    /**
     * @brief Adds text to a list to draw
     *
     * @param text Text to add
     * @param position Position of text
     * @param color Color of text
     * @param layer Layer of text
     */
    virtual auto add_text(std::string text, mathfu::Vector<float, 2> position,
                          Rendering::Color color, float layer) -> void {
        stringVector.push_back({text, position, color, layer});
    }

    /**
     * @brief Clears the list of text data
     *
     */
    virtual auto clear_tiles() -> void override {
        stringVector.clear();
        stringVector.shrink_to_fit();

        FixedTilemap<N>::clear_tiles();
    }

    /**
     * @brief Generate the tile map
     *
     */
    virtual auto generate_map() -> void override {
        if (size_map == NULL)
            return;

        FixedTilemap<N>::clear_tiles();

        for (auto &s : stringVector) {
            auto pos = s.pos;
            for (int i = 0; i < s.text.length(); i++) {
                auto c = static_cast<u32>(s.text[i]);

                if (c < 0) {
                    c = 0;
                } else if (c >= 127) {
                    c = 127;
                }

                FixedTilemap<N>::add_tile(
                    {{pos, mathfu::Vector<float, 2>(8 * scale_factor, 8 * scale_factor)},
                     s.color,
                     static_cast<u16>(c),
                     s.layer});
                pos.x += size_map[c] * scale_factor;
            }
        }

        FixedTilemap<N>::generate_map();
    }

    /**
     * @brief Calculates the necessary size for a text object (useful for center
     * / align)
     *
     * @param text Text information
     * @return float Space needed
     */
    virtual auto calculate_size(std::string text) -> float {
        if (size_map == nullptr)
            return 0;

        float x = 0;

        for (int i = 0; i < text.length(); i++) {
            auto c = static_cast<u32>(text[i]);
            x += size_map[c] * scale_factor;
        }

        return x;
    }

    /**
     * @brief This is the kerning map -- sometimes may want to set this
     * TODO: Make this accessible in a different manner
     *
     */
    float *size_map;

    /**
     * @brief Scale size of the fontmap
     * TODO: Make this also accessible in a different manner
     *
     */
    float scale_factor;

  protected:
#if USE_EASTL
    eastl::vector<TextData> stringVector;
#else
    std::vector<TextData> stringVector;
#endif
};

} // namespace Stardust_Celeste::Graphics::G2D
