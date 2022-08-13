#pragma once

#include "Utilities/Assertion.hpp"
#include "Utilities/Logger.hpp"
#include "Utilities/Singleton.hpp"
#include "Utilities/Types.hpp"

namespace Stardust_Celeste::Audio {
class AudioContext : public Singleton {
  public:
    AudioContext();
    ~AudioContext();

    inline static auto get() -> AudioContext & {
        static AudioContext actx;
        return actx;
    }

    auto initialize() -> void;
    auto terminate() -> void;

  private:
};
} // namespace Stardust_Celeste::Audio