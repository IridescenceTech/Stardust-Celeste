#pragma once
#include <Rendering/Mesh.hpp>
#include <Utilities/Types.hpp>

namespace Stardust_Celeste::Graphics::G2D {

/**
 * @brief Draws a 2D Rectangular Sprite
 *
 */
class Sprite {
  public:
    /**
     * @brief Construct a new Sprite object
     *
     * @param texture Texture ID
     * @param bounds Bounding rectangle
     */
    Sprite(u32 texture, Rendering::Rectangle bounds);
    /**
     * @brief Construct a new Sprite object
     *
     * @param texture Texture ID
     * @param bounds Bounding rectangle
     * @param selection Selection rectangle in texture coordinates
     */
    Sprite(u32 texture, Rendering::Rectangle bounds,
           Rendering::Rectangle selection);

    /**
     * @brief Construct a new Sprite object
     *
     * @param texture Texture ID
     * @param bounds Bounding rectangle
     * @param selection Selection rectangle in texture coordinates
     * @param color Tint color
     */
    Sprite(u32 texture, Rendering::Rectangle bounds,
           Rendering::Rectangle selection, Rendering::Color color);

    /**
     * @brief Construct a new Sprite object
     *
     * @param texture Texture ID
     * @param bounds Bounding rectangle
     * @param color Tint color
     */
    Sprite(u32 texture, Rendering::Rectangle bounds, Rendering::Color color);

    /**
     * @brief Destroy the Sprite object
     *
     */
    virtual ~Sprite();

    /**
     * @brief Update method -- this does nothing in the base class, but is
     * useful for animations and transitions
     *
     * @param dt Delta Time
     */
    virtual auto update(double dt) -> void;

    /**
     * @brief Draws the sprite to the screen
     *
     */
    virtual auto draw() -> void;

    /**
     * @brief Set the position of the sprite
     *
     * @param position Sprite Position XY
     */
    virtual auto set_position(Math::Vector2<float> position) -> void;

    /**
     * @brief Set the size of the sprite
     *
     * @param size Sprite Size WH
     */
    virtual auto set_size(Math::Vector2<float> size) -> void;

    /**
     * @brief Set the rectangle boundaries
     *
     * @param bounds Boundaries of the rectangle
     */
    virtual auto set_rect(Rendering::Rectangle bounds) -> void;

    /**
     * @brief Set the selection area for texture slicing
     *
     * @param selection Boundaries of the selection area
     */
    auto set_selection(Rendering::Rectangle selection) -> void;

    /**
     * @brief Set the layer
     *
     * @param layer Layer of the sprite
     */
    virtual auto set_layer(s16 layer) -> void;

    /**
     * @brief Set the tint color of the sprite
     *
     * @param color Tint color
     */
    virtual auto set_color(Rendering::Color color) -> void;

    /**
     * @brief Texture ID
     *
     */
    u32 texture;

  protected:
    virtual auto update_mesh() -> void;
    Rendering::Rectangle selection;
    Rendering::Rectangle bounds;
    Rendering::Color color;
    s16 layer;
    ScopePtr<Rendering::FixedMesh<Rendering::Vertex, 4, 6>> mesh;
};

} // namespace Stardust_Celeste::Graphics::G2D