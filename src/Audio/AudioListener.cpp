#include <Audio/AudioListener.hpp>

#ifndef PSP
#include <AL/al.h>
#endif

namespace Stardust_Celeste::Audio {

auto Listener::update(const glm::vec3 &&pos, const glm::vec3 &&vel,
                      const glm::vec3 &&look, const glm::vec3 &&up) -> void {
#ifndef PSP
    ALfloat orientation[6] = {look.x, look.y, look.z, up.x, up.y, up.z};
    alListener3f(AL_POSITION, pos.x, pos.y, pos.z);
    // check for errors
    alListener3f(AL_VELOCITY, vel.x, vel.y, vel.z);
    // check for errors
    alListenerfv(AL_ORIENTATION, orientation);
#endif
}
} // namespace Stardust_Celeste::Audio
