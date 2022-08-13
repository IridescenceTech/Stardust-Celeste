#pragma once

#ifndef PSP
#include "AL/al.h"
#endif

#include <string>
#include <glm.hpp>

namespace Stardust_Celeste::Audio
{
    class Clip
    {
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
#endif
    };
}