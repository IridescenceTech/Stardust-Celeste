#include <Rendering/GI.hpp>
#include <Rendering/Primitive/Rectangle.hpp>

namespace Stardust_Celeste::Rendering::Primitive {

Rectangle::Rectangle(Rendering::Rectangle bnd, Rendering::Color col, float lay)
    : bound(bnd), color(col), layer(lay) {
    verts = nullptr;
    idxs = nullptr;
    mesh = nullptr;

    build_mesh();
}

Rectangle::~Rectangle() {
    delete[] verts;
    delete[] idxs;
}

void Rectangle::draw() {
    GI::disable(GI_TEXTURE_2D);

    mesh->draw();

    GI::enable(GI_TEXTURE_2D);
}

void Rectangle::build_mesh() {
    if (verts == nullptr)
        verts = new Rendering::Vertex[4];

    verts[0] = Rendering::Vertex{
        0, 0, color, bound.position.x, bound.position.y, (float)layer};
    verts[1] = Rendering::Vertex{0,
                                 0,
                                 color,
                                 bound.position.x + bound.extent.x,
                                 bound.position.y,
                                 (float)layer};
    verts[2] = Rendering::Vertex{0,
                                 0,
                                 color,
                                 bound.position.x + bound.extent.x,
                                 bound.position.y + bound.extent.y,
                                 (float)layer};
    verts[3] = Rendering::Vertex{0,
                                 0,
                                 color,
                                 bound.position.x,
                                 bound.position.y + bound.extent.y,
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

} // namespace Stardust_Celeste::Rendering::Primitive