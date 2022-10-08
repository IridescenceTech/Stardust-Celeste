#include <Graphics/2D/Tilemap.hpp>

namespace Stardust_Celeste::Graphics::G2D {

struct AnimatedTile : public Tile{
    u16 start_idx, final_idx;
};

class AnimatedTilemap : public Tilemap {
  public:
    AnimatedTilemap(u32 texture, glm::vec2 atlasSize);
    ~AnimatedTilemap();

    auto add_tile(AnimatedTile tile) -> void;
    auto add_tiles(std::vector<AnimatedTile> tiles) -> void;
    auto clear_tiles() -> void;

    auto update(double dt) -> void;

    auto tick() -> void;
    auto generate_map() -> void;

    float tickPerSec;
private:
    float atime;
    std::vector<AnimatedTile> atileMap;
};

} // namespace Stardust_Celeste::Graphics::G2D
