#include <Audio/Clip.hpp>
#define DR_WAV_IMPLEMENTATION
#include <Audio/dr_wav.h>
#include <stdexcept>

namespace Stardust_Celeste::Audio {

Clip::Clip(std::string path) {
#ifndef PSP
    alGenSources((ALuint)1, &source);

    alSourcef(source, AL_PITCH, 1);
    alSourcef(source, AL_GAIN, 1);
    alSource3f(source, AL_POSITION, 0, 0, 0);
    alSource3f(source, AL_VELOCITY, 0, 0, 0);
    alSourcei(source, AL_LOOPING, AL_FALSE);

    alGenBuffers((ALuint)1, &buffer);

    drwav wav;
    if (!drwav_init_file(&wav, path.c_str(), NULL)) {
        throw std::runtime_error("Could not load sound file: " + path);
    }

    size_t size =
        (size_t)wav.totalPCMFrameCount * wav.channels * sizeof(int16_t);
    int16_t *pSampleData = (int16_t *)malloc(size);
    drwav_read_pcm_frames_s16(&wav, wav.totalPCMFrameCount, pSampleData);

    // At this point pSampleData contains every decoded sample as signed 32-bit
    // PCM.
    alBufferData(buffer, AL_FORMAT_MONO16, pSampleData, size, wav.sampleRate);

    drwav_uninit(&wav);
    alSourcei(source, AL_BUFFER, buffer);
#else
    sound = oslLoadSoundFile(path.c_str(), OSL_FMT_NONE);
#endif
}
Clip::~Clip() {
#ifndef PSP
    alDeleteSources(1, &source);
    alDeleteBuffers(1, &buffer);
#else
    oslDeleteSound(sound);
#endif
}

auto Clip::set_pitch(float val) -> void {
#ifndef PSP
    alSourcef(source, AL_PITCH, val);
#endif
}
auto Clip::set_volume(float val) -> void {
#ifndef PSP
    alSourcef(source, AL_GAIN, val);
#endif
}

auto Clip::set_position(glm::vec3 position) -> void {
#ifndef PSP
    alSource3f(source, AL_POSITION, position.x, position.y, position.z);
#endif
}
auto Clip::set_velocity(glm::vec3 velocity) -> void {
#ifndef PSP
    alSource3f(source, AL_VELOCITY, velocity.x, velocity.y, velocity.z);
#endif
}
auto Clip::set_looping(bool looping) -> void {
#ifndef PSP
    alSourcei(source, AL_LOOPING, looping);
#else
    oslSetSoundLoop(sound, looping);
#endif
}

auto Clip::play() -> void {
#ifndef PSP
    alSourcePlay(source);
#else
    oslPlaySound(sound, 0);
#endif
}
auto Clip::pause() -> void {
#ifndef PSP
    alSourcePause(source);
#else
    oslPauseSound(sound, -1);
#endif
}
auto Clip::stop() -> void {
#ifndef PSP
    alSourceStop(source);
#else
    oslStopSound(sound);
#endif
}
} // namespace Stardust_Celeste::Audio