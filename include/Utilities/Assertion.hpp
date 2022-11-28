#pragma once
#include "../Platform/Platform.hpp"
#include "Logger.hpp"

#if BUILD_PLAT == BUILD_POSIX
#include <signal.h>
#endif

inline auto SC_DEBUG_BREAK() -> void {

#if BUILD_PLAT == BUILD_WINDOWS
    __debugbreak();
#elif BUILD_PLAT == BUILD_POSIX
    raise(SIGTRAP);
#elif BUILD_PLAT == BUILD_PSP
    asm(".set noreorder\n"
        "break \n"
        "jr    $31\n"
        "nop\n");
#elif BUILD_PLAT == BUILD_VITA
    // TODO: figure this out some time
#elif BUILD_PLAT == BUILD_3DS
    // TODO: Figure this out later
#else
#warning No Debug Break!
#endif
}

#define SC_ASSERT(check, ...)                                                  \
    if (!(check)) {                                                            \
        SC_APP_ERROR(__VA_ARGS__);                                             \
        SC_DEBUG_BREAK();                                                      \
    }
#define SC_CORE_ASSERT(check, ...)                                             \
    if (!(check)) {                                                            \
        SC_CORE_ERROR(__VA_ARGS__);                                            \
        SC_DEBUG_BREAK();                                                      \
    }
