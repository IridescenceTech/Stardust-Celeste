#pragma once

#include "Utilities/Assertion.hpp"
#include "Utilities/Logger.hpp"
#include "Utilities/Singleton.hpp"
#include "Utilities/Types.hpp"

namespace Stardust_Celeste::Audio {

/**
 * @brief The audio context is a static singleton which is used to control the
 * overall context of the application There is no reason to use this outside of
 * internal platform code.
 */
class AudioContext final : public Singleton {
  public:
    AudioContext() = default;
    ~AudioContext() = default;

    /**
     * @brief Returns the static audio context
     *
     * @return AudioContext& Audio Context Object
     */
    inline static auto get() -> AudioContext & {
        static AudioContext actx;
        return actx;
    }

    /**
     * @brief Initializes the current audio context
     *
     */
    auto initialize() -> void;

    /**
     * @brief Terminates the current audio context
     *
     */
    auto terminate() -> void;
};
} // namespace Stardust_Celeste::Audio