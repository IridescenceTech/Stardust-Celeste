#include <Audio/AudioContext.hpp>
//#include <SDL2/SDL.h>
#if BUILD_PLAT == BUILD_3DS
//#include <SDL/SDL_mixer.h>
#else
//#include <SDL2/SDL_mixer.h>
#endif

namespace Stardust_Celeste::Audio {

auto AudioContext::initialize() -> void {
    /*
#if BUILD_PLAT != BUILD_3DS
    if (SDL_Init(SDL_INIT_AUDIO) < 0)
        throw std::runtime_error("Audio system initialization failed!");

#if PSP || VITA
    if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) == -1)
#else
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 6, 4096) == -1)
#endif
        throw std::runtime_error("Audio channel initialization failed!");

#endif
     */
}

auto AudioContext::terminate() noexcept -> void {
    /*
#if BUILD_PLAT != BUILD_3DS
    Mix_CloseAudio();
#endif
     */
}

} // namespace Stardust_Celeste::Audio