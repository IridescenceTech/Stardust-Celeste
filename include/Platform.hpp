/**
 * @file Platform.hpp
 * @author Nathan Bourgeois <iridescentrosesfall@gmail.com>
 * @brief
 * @version 0.1
 * @date 2021-12-15
 *
 * @copyright Copyright (c) 2021
 *
 */
#pragma once

#ifndef BUILD_PLAT
#error No Build Platform Selected!
#endif

#define BUILD_WINDOWS 0
#define BUILD_POSIX 1
#define BUILD_PSP 2
#define BUILD_VITA 3
#define BUILD_PS2 4
#define BUILD_PS3 5
#define BUILD_GAMECUBE 6
#define BUILD_3DS 7
#define BUILD_SWITCH 8

#if BUILD_PLAT == BUILD_VITA
#error PLATFORM NOT SUPPORTED
#elif BUILD_PLAT == BUILD_PS2
#error PLATFORM NOT SUPPORTED
#elif BUILD_PLAT == BUILD_PS3
#error PLATFORM NOT SUPPORTED
#elif BUILD_PLAT == BUILD_GAMECUBE
#error PLATFORM NOT SUPPORTED
#elif BUILD_PLAT == BUILD_3DS
#error PLATFORM NOT SUPPORTED
#elif BUILD_PLAT == BUILD_SWITCH
#error PLATFORM NOT SUPPORTED
#endif

namespace Stardust_Celeste {
enum class PlatformType {
  Windows = 0,
  Posix = 1,
  Psp = 2,
  Psvita = 3,
  Ps2 = 4,
  Ps3 = 5,
  Gamecube = 6,
  N3DS = 7,
  NSwitch = 8,
};

#if BUILD_PLAT == BUILD_WINDOWS
constexpr auto BUILD_PLATFORM = Stardust_Celeste::PlatformType::Windows;
#elif BUILD_PLAT == BUILD_POSIX
constexpr auto BUILD_PLATFORM = Stardust_Celeste::PlatformType::Posix;
#elif BUILD_PLAT == BUILD_PSP
constexpr auto BUILD_PLATFORM = Stardust_Celeste::PlatformType::Psp;
#else
#error Invalid Platform!
#endif
} // namespace Stardust_Celeste
