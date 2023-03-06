#include <Graphics/2D/Sprite.hpp>
#include <Rendering/Texture.hpp>
#include <Utilities/Assertion.hpp>

namespace Stardust_Celeste::Graphics::G2D {

Sprite::Sprite(u32 tex, Rendering::Rectangle bnd) {
    SC_CORE_ASSERT(tex != 0, "Sprite construction: Texture ID is 0!");
    texture = tex;
    bounds = bnd;
    selection = Rendering::Rectangle{{0, 0}, {1, 1}};
    color = Rendering::Color{255, 255, 255, 255};

    layer = 0;
    update_mesh();
}

Sprite::Sprite(u32 tex, Rendering::Rectangle bnd, Rendering::Rectangle sel) {
    SC_CORE_ASSERT(tex != 0, "Sprite construction: Texture ID is 0!");
    texture = tex;
    bounds = bnd;
    selection = sel;
    color = Rendering::Color{255, 255, 255, 255};

    layer = 0;
    update_mesh();
}

Sprite::Sprite(u32 tex, Rendering::Rectangle bnd, Rendering::Rectangle sel,
               Rendering::Color col) {
    SC_CORE_ASSERT(tex != 0, "Sprite construction: Texture ID is 0!");

    texture = tex;
    bounds = bnd;
    selection = sel;
    color = col;

    layer = 0;
    update_mesh();
}
Sprite::Sprite(u32 tex, Rendering::Rectangle bnd, Rendering::Color col) {
    SC_CORE_ASSERT(tex != 0, "Sprite construction: Texture ID is 0!");
    texture = tex;
    bounds = bnd;
    selection = Rendering::Rectangle{{0, 0}, {1, 1}};
    color = col;

    layer = 0;
    update_mesh();
}

Sprite::~Sprite() { mesh->delete_data(); }

auto Sprite::update(double dt) -> void {
    // For base: do nothing.
}

auto Sprite::draw() -> void {
    Rendering::TextureManager::get().bind_texture(texture);
    mesh->draw();
}

auto Sprite::set_position(Math::Vector2<float> position) -> void {
    bounds.position = position;
    update_mesh();
}

auto Sprite::set_size(Math::Vector2<float> size) -> void {
    bounds.extent = size;
    update_mesh();
}

auto Sprite::set_rect(Rendering::Rectangle bnd) -> void {
    bounds = bnd;
    update_mesh();
}

auto Sprite::set_selection(Rendering::Rectangle sel) -> void {
    selection = sel;
    update_mesh();
}

auto Sprite::set_layer(s16 layer) -> void {
    this->layer = layer;
    update_mesh();
}

auto Sprite::set_color(Rendering::Color col) -> void {
    color = col;
    update_mesh();
}

auto Sprite::update_mesh() -> void {
    if (mesh.get() == nullptr)
        mesh = create_scopeptr<Rendering::FixedMesh<Rendering::Vertex, 4, 6>>();

    mesh->vertices[0] = Rendering::Vertex{
        selection.position.x, selection.position.y + selection.extent.y, color,
        bounds.position.x,    bounds.position.y,    (float)layer};
    mesh->vertices[1] =
        Rendering::Vertex{selection.position.x + selection.extent.x,
                          selection.position.y + selection.extent.y,
                          color,
                          bounds.position.x + bounds.extent.x,
                          bounds.position.y,
                          (float)layer};
    mesh->vertices[2] =
        Rendering::Vertex{selection.position.x + selection.extent.x,
                          selection.position.y ,
                          color,
                          bounds.position.x + bounds.extent.x,
                          bounds.position.y + bounds.extent.y,
                          (float)layer};
    mesh->vertices[3] =
        Rendering::Vertex{selection.position.x ,
                          selection.position.y ,
                          color,
                          bounds.position.x,
                          bounds.position.y + bounds.extent.y,
                          (float)layer};

#if PSP
    auto tInfo = Rendering::TextureManager::get().get_texture(texture);

    float wRatio = 1.0f;
    float hRatio = 1.0f;

    if (tInfo != nullptr) {
        wRatio = (float)tInfo->width / (float)tInfo->pW;
        hRatio = (float)tInfo->height / (float)tInfo->pH;
    }

    for (int i = 0; i < 4; i++) {
        mesh->vertices[i].u *= wRatio;
        mesh->vertices[i].v *= hRatio;
    }
#endif

    mesh->indices[0] = 0;
    mesh->indices[1] = 1;
    mesh->indices[2] = 2;
    mesh->indices[3] = 2;
    mesh->indices[4] = 3;
    mesh->indices[5] = 0;

    mesh->setup_buffer();
}

} // namespace Stardust_Celeste::Graphics::G2D