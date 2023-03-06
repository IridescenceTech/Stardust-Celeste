#pragma once
#include <Graphics/2D/Tilemap.hpp>
#include <Rendering/Mesh.hpp>
#include <Utilities/Types.hpp>
#include <map>
#include <vector>

namespace Stardust_Celeste::Graphics::G2D {

/**
 * @brief Defines the text data required to render text to the screen
 *
 */
struct TextData {
    std::string text;
    Math::Vector2<float> pos;
    Rendering::Color color;
    float layer;
};

/**
 * @brief Font Rendering Tilemap
 *
 */
class FontRenderer : public Tilemap {
  public:
    /**
     * @brief Construct a new Font Renderer object
     *
     * @param texture Texture ID
     * @param atlasSize Texture Atlas Size
     */
    FontRenderer(u32 texture, Math::Vector2<float> atlasSize);

    /**
     * @brief Destroy the Font Renderer object
     *
     */
    virtual ~FontRenderer();

    /**
     * @brief Adds text to a list to draw
     *
     * @param text Text to add
     * @param position Position of text
     * @param color Color of text
     * @param layer Layer of text
     */
    virtual auto add_text(std::string text, Math::Vector2<float> position,
                          Rendering::Color color, float layer) -> void;

    /**
     * @brief Clears the list of text data
     *
     */
    virtual auto clear_tiles() -> void override;

    /**
     * @brief Generate the tile map
     *
     */
    virtual auto generate_map() -> void override;

    /**
     * @brief Calculates the necessary size for a text object (useful for center
     * / align)
     *
     * @param text Text information
     * @return float Space needed
     */
    virtual auto calculate_size(std::string text) -> float;

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
