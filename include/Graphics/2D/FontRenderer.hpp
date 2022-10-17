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
    glm::vec2 pos;
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
    FontRenderer(u32 texture, glm::vec2 atlasSize);

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
    auto add_text(std::string text, glm::vec2 position, Rendering::Color color,
                  float layer) -> void;
    
    /**
     * @brief Clears the list of text data
     * 
     */
    auto clear() -> void;

    /**
     * @brief Draw the map to the screen
     * 
     */
    auto draw() -> void;

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

    /**
     * @brief Generate the tile map
     * 
     */
    auto rebuild() -> void;

    /**
     * @brief Calculates the necessary size for a text object (useful for center / align)
     * 
     * @param text Text information
     * @return float Space needed
     */
    auto calculate_size(std::string text) -> float;

  protected:
    std::vector<TextData> stringVector;
};

} // namespace Stardust_Celeste::Graphics::G2D
