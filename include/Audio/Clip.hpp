#pragma once

#include <Utilities/Types.hpp>
#include <glm.hpp>
#include <string>

#ifndef PSP
#include "AL/al.h"
#include <ext/stb_vorbis.h>
#else
#include <osl_sound/audio.h>
#include <osl_sound/oslib.h>
#endif

namespace Stardust_Celeste::Audio {
class Clip {
  public:
    Clip(std::string path, bool stream = false);
    ~Clip();

    auto set_pitch(float val) -> void;
    auto set_volume(float val) -> void;

    auto set_position(glm::vec3 position) -> void;
    auto set_velocity(glm::vec3 velocity) -> void;
    auto set_looping(bool looping) -> void;

#ifndef PSP
    auto streamData(ALuint buffer) -> bool;
#endif

    auto update() -> void;

    auto play() -> void;
    auto pause() -> void;
    auto stop() -> void;

  private:
    bool isStreaming;
    bool shouldLoop;
#ifndef PSP
    ALuint ID;

    stb_vorbis *stream;
    stb_vorbis_info info;

    ALuint buffers[2];
    ALuint source;
    ALenum format;

    size_t bufferSize;

    size_t totalSamplesLeft;

#else
    OSL_SOUND *sound;
#endif
};
} // namespace Stardust_Celeste::Audio