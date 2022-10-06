#pragma once
#include <glm.hpp>

namespace Stardust_Celeste::Audio {
/**
 * @brief The audio listener is a positionable Audio Listener in the world. This
 * listener controls the attenuation, panning, and effect of sound.
 *
 * It is currently non-extensible
 */
class Listener final {
  public:
    Listener() = default;
    ~Listener() = default;

    /**
     * @brief Updates the current position of the audio listener
     *
     * @param pos Position of the listener
     * @param vel Velocity of the listener
     * @param look Direction of the listener
     * @param up Up vector of the listener
     */
    auto update(const glm::vec3 &&pos, const glm::vec3 &&vel,
                const glm::vec3 &&look, const glm::vec3 &&up) -> void;

  private:
};
} // namespace Stardust_Celeste::Audio
