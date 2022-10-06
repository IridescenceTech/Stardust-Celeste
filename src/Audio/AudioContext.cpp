#include <Audio/AudioContext.hpp>

#ifndef PSP
#include <AL/al.h>
#include <AL/alc.h>
#else
#include <osl_sound/audio.h>
#include <osl_sound/oslib.h>
#endif

namespace Stardust_Celeste::Audio {

#ifndef PSP
ALCdevice *device;
ALCcontext *context;

#endif

auto AudioContext::initialize() -> void {
#ifndef PSP
    device = alcOpenDevice(NULL);
    if (!device)
        throw std::runtime_error("Could not find an audio device!");

    context = alcCreateContext(device, NULL);
    if (!alcMakeContextCurrent(context))
        throw std::runtime_error("Could not make an audio context!");

    SC_CORE_INFO("OpenAL Initialized!");

    ALfloat listenerOri[] = {0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f};

    alListener3f(AL_POSITION, 0, 0, 1.0f);
    // check for errors
    alListener3f(AL_VELOCITY, 0, 0, 0);
    // check for errors
    alListenerfv(AL_ORIENTATION, listenerOri);
    // check for errors
#else
    VirtualFileInit();
    oslInitAudio();
#endif
}
auto AudioContext::terminate() -> void {
#ifndef PSP
    device = alcGetContextsDevice(context);
    alcMakeContextCurrent(NULL);
    alcDestroyContext(context);
    alcCloseDevice(device);
#else
    oslDeinitAudio();
#endif
}
} // namespace Stardust_Celeste::Audio