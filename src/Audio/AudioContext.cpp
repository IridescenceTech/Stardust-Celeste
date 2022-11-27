#include <Audio/AudioContext.hpp>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

namespace Stardust_Celeste::Audio {

auto AudioContext::initialize() -> void {
    if (SDL_Init(SDL_INIT_AUDIO) < 0)
        throw std::runtime_error("Audio system initialization failed!");

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 6, 4096) == -1)
        throw std::runtime_error("Audio channel initialization failed!");
}

auto AudioContext::terminate() noexcept -> void { Mix_CloseAudio(); }

} // namespace Stardust_Celeste::Audio