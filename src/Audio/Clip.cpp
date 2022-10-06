#include <Audio/Clip.hpp>
#define DR_WAV_IMPLEMENTATION
#include <Audio/dr_wav.h>
#include <Utilities/Logger.hpp>
#include <stdexcept>

#if BUILD_PLAT == BUILD_VITA
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>
OggVorbis_File vf;
vorbis_info *vi;
int current_section = 0;

#endif

namespace Stardust_Celeste::Audio {

#ifndef PSP
auto Clip::streamData(ALuint buffer) -> bool {

#define BUFFER_SIZE 4096 * 32
#if BUILD_PLAT == BUILD_VITA
    char pcm[BUFFER_SIZE];

    int size = 0;
    int result = 0;
    while (size < BUFFER_SIZE) {
        result = ov_read(&vf, pcm + size, BUFFER_SIZE - size, 0, 2, 1,
                         &current_section);
        if (result > 0)
            size += result;
        else
            break;
    }

    if (size == 0)
        return false;

    alBufferData(buffer, format, pcm, size, vi->rate);
#else
    ALshort pcm[BUFFER_SIZE];
    int size = 0;
    int result = 0;

    while (size < BUFFER_SIZE) {
        result = stb_vorbis_get_samples_short_interleaved(
            stream, info.channels, pcm + size, BUFFER_SIZE - size);
        if (result > 0)
            size += result * info.channels;
        else
            break;
    }

    if (size == 0)
        return false;

    alBufferData(buffer, format, pcm, size * sizeof(ALshort), info.sample_rate);
    totalSamplesLeft -= size;
#endif

#undef BUFFER_SIZE
    return true;
}
#endif

Clip::Clip(const std::string &&path, bool s) {
    isStreaming = s;
#ifndef PSP
    alGenSources((ALuint)1, &source);

    alSourcef(source, AL_PITCH, 1);
    alSourcef(source, AL_GAIN, 1);
    alSource3f(source, AL_POSITION, 0, 0, 0);
    alSource3f(source, AL_VELOCITY, 0, 0, 0);
    alSourcei(source, AL_LOOPING, AL_FALSE);

    if (!s) {
        alGenBuffers((ALuint)1, &buffers[0]);

        drwav wav;
        if (!drwav_init_file(&wav, path.c_str(), NULL)) {
            throw std::runtime_error("Could not load sound file: " + path);
        }

        size_t size =
            (size_t)wav.totalPCMFrameCount * wav.channels * sizeof(int16_t);
        int16_t *pSampleData = (int16_t *)malloc(size);
        drwav_read_pcm_frames_s16(&wav, wav.totalPCMFrameCount, pSampleData);

        // At this point pSampleData contains every decoded sample as signed
        // 32-bit PCM.
        alBufferData(buffers[0], AL_FORMAT_MONO16, pSampleData, size,
                     wav.sampleRate);

        drwav_uninit(&wav);
        alSourcei(source, AL_BUFFER, buffers[0]);
    } else {
#if BUILD_PLAT == BUILD_VITA
        alGenBuffers(2, buffers);
        FILE *fp = fopen(path.c_str(), "rb");

        if (fp == NULL) {
            SC_CORE_INFO("FAILED TO LOAD OGG FILE");
        }
        if (ov_open_callbacks(fp, &vf, NULL, 0, OV_CALLBACKS_DEFAULT) < 0) {
            exit(1);
        }

        vi = ov_info(&vf, -1);

        if (vi->channels == 2)
            format = AL_FORMAT_STEREO16;
        else
            format = AL_FORMAT_MONO16;

        streamData(buffers[0]);
        streamData(buffers[1]);

        alSourceQueueBuffers(source, 2, buffers);
        alSourcePlay(source);

        totalSamplesLeft = 1;
#else
        alGenBuffers(2, buffers);
        bufferSize = 4096 * 8;
        shouldLoop = false;

        stream = stb_vorbis_open_filename(path.c_str(), NULL, NULL);

        if (!stream) {
            SC_CORE_INFO("FAILED TO LOAD OGG FILE");
            throw std::runtime_error("Could not load streaming sound file: " +
                                     path);
        }

        info = stb_vorbis_get_info(stream);
        if (info.channels == 2)
            format = AL_FORMAT_STEREO16;
        else
            format = AL_FORMAT_MONO16;

        streamData(buffers[0]);
        streamData(buffers[1]);

        alSourceQueueBuffers(source, 2, buffers);
        alSourcePlay(source);

        totalSamplesLeft =
            stb_vorbis_stream_length_in_samples(stream) * info.channels;
#endif
    }
#else
    auto fmt = s ? OSL_FMT_STREAM : OSL_FMT_NONE;
    sound = oslLoadSoundFile(path.c_str(), OSL_FMT_NONE);
#endif
}
Clip::~Clip() {
#ifndef PSP
    alDeleteSources(1, &source);
    if (isStreaming) {
        stb_vorbis_close(stream);
        alDeleteBuffers(2, buffers);
    } else {
        alDeleteBuffers(1, &buffers[0]);
    }
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

auto Clip::set_position(const glm::vec3 &&position) -> void {
#ifndef PSP
    alSource3f(source, AL_POSITION, position.x, position.y, position.z);
#endif
}
auto Clip::set_velocity(const glm::vec3 &&velocity) -> void {
#ifndef PSP
    alSource3f(source, AL_VELOCITY, velocity.x, velocity.y, velocity.z);
#endif
}
auto Clip::set_looping(bool looping) -> void {
    shouldLoop = looping;
#ifndef PSP
    alSourcei(source, AL_LOOPING, shouldLoop);
#else
    oslSetSoundLoop(sound, shouldLoop);
#endif
}

auto Clip::play(const uint32_t channel) -> void {
#ifndef PSP
    alSourcePlay(source);
#else
    oslPlaySound(sound, channel);
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

auto Clip::update() -> void {
#ifndef PSP
    ALint processed = 0;
    alGetSourcei(source, AL_BUFFERS_PROCESSED, &processed);

    while (processed--) {
        ALuint buffer = 0;

        alSourceUnqueueBuffers(source, 1, &buffer);

        if (!streamData(buffer)) {
            bool shouldExit = true;

#if BUILD_PLAT != BUILD_VITA
            if (shouldLoop) {
                stb_vorbis_seek_start(stream);
                totalSamplesLeft =
                    stb_vorbis_stream_length_in_samples(stream) * info.channels;
                shouldExit = !streamData(buffer);
            }
#endif

            if (shouldExit)
                return;
        }
        alSourceQueueBuffers(source, 1, &buffer);
    }
#endif
}

} // namespace Stardust_Celeste::Audio