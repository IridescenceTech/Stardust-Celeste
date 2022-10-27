#include <Rendering/GI.hpp>
#include <Rendering/Primitive/Rectangle.hpp>

namespace Stardust_Celeste::Rendering::Primitive {

Rectangle::Rectangle(Rendering::Rectangle bnd, Rendering::Color col, float lay)
    : bound(bnd), color(col), layer(lay) {
    mesh = nullptr;

    build_mesh();
}

Rectangle::~Rectangle() { mesh->delete_data(); }

void Rectangle::draw() {
    GI::disable(GI_TEXTURE_2D);

    mesh->draw();

    GI::enable(GI_TEXTURE_2D);
}

void Rectangle::build_mesh() {

    if (mesh == nullptr) {
        mesh = create_scopeptr<Rendering::FixedMesh<Rendering::Vertex, 4, 6>>();
    }

    mesh->vertices[0] = Rendering::Vertex{
        0, 0, color, bound.position.x, bound.position.y, (float)layer};
    mesh->vertices[1] = Rendering::Vertex{0,
                                          0,
                                          color,
                                          bound.position.x + bound.extent.x,
                                          bound.position.y,
                                          (float)layer};
    mesh->vertices[2] = Rendering::Vertex{0,
                                          0,
                                          color,
                                          bound.position.x + bound.extent.x,
                                          bound.position.y + bound.extent.y,
                                          (float)layer};
    mesh->vertices[3] = Rendering::Vertex{0,
                                          0,
                                          color,
                                          bound.position.x,
                                          bound.position.y + bound.extent.y,
                                          (float)layer};

    mesh->indices[0] = (0);
    mesh->indices[1] = (1);
    mesh->indices[2] = (2);
    mesh->indices[3] = (2);
    mesh->indices[4] = (3);
    mesh->indices[5] = (0);

    mesh->delete_data();
    mesh->setup_buffer();
}

} // namespace Stardust_Celeste::Rendering::Primitive