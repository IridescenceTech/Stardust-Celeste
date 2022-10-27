#pragma once

#include "../Rendering.hpp"

namespace Stardust_Celeste::Rendering::Primitive {

class Rectangle {
  public:
    Rectangle(Rendering::Rectangle bounds, Rendering::Color color, float layer);
    ~Rectangle();

    void draw();

  private:
    Rendering::Rectangle bound;
    Rendering::Color color;
    int layer;

    void build_mesh();

    ScopePtr<Rendering::FixedMesh<Rendering::Vertex, 4, 6>> mesh;
};
} // namespace Stardust_Celeste::Rendering::Primitive