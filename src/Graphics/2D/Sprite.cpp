#include <Graphics/2D/Sprite.hpp>
#include <Rendering/Texture.hpp>

namespace Stardust_Celeste::Graphics::G2D {

Sprite::Sprite(u32 tex, Rendering::Rectangle bnd) {
    texture = tex;
    bounds = bnd;
    selection = Rendering::Rectangle{{0, 0}, {1, 1}};
    color = Rendering::Color{255, 255, 255, 255};

    verts = nullptr;
    idxs = nullptr;

    layer = 0;
    update_mesh();
}

Sprite::Sprite(u32 tex, Rendering::Rectangle bnd, Rendering::Rectangle sel) {
    texture = tex;
    bounds = bnd;
    selection = sel;
    color = Rendering::Color{255, 255, 255, 255};

    verts = nullptr;
    idxs = nullptr;

    layer = 0;
    update_mesh();
}

Sprite::Sprite(u32 tex, Rendering::Rectangle bnd, Rendering::Rectangle sel,
               Rendering::Color col) {

    texture = tex;
    bounds = bnd;
    selection = sel;
    color = col;

    verts = nullptr;
    idxs = nullptr;

    layer = 0;
    update_mesh();
}
Sprite::Sprite(u32 tex, Rendering::Rectangle bnd, Rendering::Color col) {
    texture = tex;
    bounds = bnd;
    selection = Rendering::Rectangle{{0, 0}, {1, 1}};
    color = col;

    verts = nullptr;
    idxs = nullptr;

    layer = 0;
    update_mesh();
}

Sprite::~Sprite() {
    delete[] verts;
    delete[] idxs;
}

auto Sprite::update(double dt) -> void {
    // For base: do nothing.
}

auto Sprite::draw() -> void {
    Rendering::TextureManager::get().bind_texture(texture);
    mesh->draw();
}

auto Sprite::set_position(glm::vec2 position) -> void {
    bounds.position = position;
    update_mesh();
}

auto Sprite::set_size(glm::vec2 size) -> void {
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
    if (verts == nullptr)
        verts = new Rendering::Vertex[4];

    verts[0] = Rendering::Vertex{
        selection.position.x , selection.position.y +selection.extent.y, color,
        bounds.position.x,    bounds.position.y + bounds.extent.y,    (float)layer};
    verts[1] = Rendering::Vertex{selection.position.x + selection.extent.x,
                                 selection.position.y + selection.extent.y,
                                 color,
                                 bounds.position.x + bounds.extent.x,
                                 bounds.position.y + bounds.extent.y,
                                 (float)layer};
    verts[2] = Rendering::Vertex{selection.position.x + selection.extent.x,
                                 selection.position.y ,
                                 color,
                                 bounds.position.x + bounds.extent.x,
                                 bounds.position.y,
                                 (float)layer};
    verts[3] = Rendering::Vertex{selection.position.x,
                                 selection.position.y ,
                                 color,
                                 bounds.position.x,
                                 bounds.position.y ,
                                 (float)layer};

    if (idxs == nullptr) {
        idxs = new u16[6];
        idxs[0] = 0;
        idxs[1] = 1;
        idxs[2] = 2;
        idxs[3] = 2;
        idxs[4] = 3;
        idxs[5] = 0;
    }

    if (mesh.get() == nullptr)
        mesh = create_scopeptr<Rendering::Mesh>(verts, 4, idxs, 6);
    else {
        mesh->delete_data();
        mesh->add_data(verts, 4, idxs, 6);
    }
}

} // namespace Stardust_Celeste::Graphics::G2D