#pragma once

#include <Utilities/Types.hpp>
#include <glm.hpp>
#include <string>

#ifndef PSP
#include "AL/al.h"
#else
#include <osl_sound/audio.h>
#include <osl_sound/oslib.h>
#endif

namespace Stardust_Celeste::Audio {
class Clip {
  public:
    Clip(std::string path);
    ~Clip();

    auto set_pitch(float val) -> void;
    auto set_volume(float val) -> void;

    auto set_position(glm::vec3 position) -> void;
    auto set_velocity(glm::vec3 velocity) -> void;
    auto set_looping(bool looping) -> void;

    auto play() -> void;
    auto pause() -> void;
    auto stop() -> void;

  private:
#ifndef PSP
    ALuint source;
    ALuint buffer;
#else
    OSL_SOUND *sound;
#endif
};
} // namespace Stardust_Celeste::Audio