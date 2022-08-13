#include <Audio/AudioContext.hpp>

#ifndef PSP
#include <AL/al.h>
#include <AL/alc.h>
#endif

namespace Stardust_Celeste::Audio
{

#ifndef PSP
    ALCdevice *device;
    ALCcontext *context;

#endif

    AudioContext::AudioContext()
    {
    }
    AudioContext::~AudioContext()
    {
    }

    auto AudioContext::initialize() -> void
    {
#ifndef PSP
        device = alcOpenDevice(NULL);
        if (!device)
            throw std::runtime_error("Could not find an audio device!");

        context = alcCreateContext(device, NULL);
        if (!alcMakeContextCurrent(context))
            throw std::runtime_error("Could not make an audio context!");

        SC_CORE_INFO("OpenAL Initialized!");
#endif
    }
    auto AudioContext::terminate() -> void
    {
#ifndef PSP
        device = alcGetContextsDevice(context);
        alcMakeContextCurrent(NULL);
        alcDestroyContext(context);
        alcCloseDevice(device);
#endif
    }
    auto AudioContext::update() -> void
    {
    }
}