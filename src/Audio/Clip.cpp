#include "Utilities/Assertion.hpp"
#include <Audio/Clip.hpp>
#include <SDL2/SDL.h>

#if BUILD_PLAT == BUILD_3DS
#include <SDL/SDL_mixer.h>
#else
#include <SDL2/SDL_mixer.h>
#endif

#include <Utilities/Logger.hpp>
#include <stdexcept>

namespace Stardust_Celeste::Audio {

Clip::Clip(const std::string &&path, bool s) {
    isStreaming = s;

    data = nullptr;
    if (s)
        data = Mix_LoadMUS(path.c_str());
    else
        data = Mix_LoadWAV(path.c_str());

    if (data == nullptr)
        throw std::runtime_error("Could not load file: " + path);

    chan = -1;
    shouldLoop = false;
}
Clip::~Clip() {
    if (isStreaming && data == nullptr)
        Mix_FreeMusic(reinterpret_cast<Mix_Music *>(data));
    else
        Mix_FreeChunk(reinterpret_cast<Mix_Chunk *>(data));
}

auto Clip::set_volume(float val) -> void {
    int v = 128 * val;

    if (v > 128)
        v = 128;

    if (v < 0)
        v = 0;

    if (isStreaming)
        Mix_VolumeMusic(v);
    else
        Mix_VolumeChunk(reinterpret_cast<Mix_Chunk *>(data), v);
}

auto Clip::set_looping(bool looping) -> void { shouldLoop = looping; }

auto Clip::play() -> void {
    int loops = 0;
    if (shouldLoop)
        loops = -1;

    if (isStreaming)
        Mix_PlayMusic(reinterpret_cast<Mix_Music *>(data), loops);
    else
        chan = Mix_PlayChannel(-1, reinterpret_cast<Mix_Chunk *>(data), loops);
}
auto Clip::pause() -> void {
    if (isStreaming)
        Mix_PauseMusic();
    else if (chan != -1)
        Mix_Pause(chan);
}
auto Clip::stop() -> void {
    if (isStreaming)
        Mix_HaltMusic();
    else if (chan != -1)
        Mix_HaltChannel(chan);
}

} // namespace Stardust_Celeste::Audio