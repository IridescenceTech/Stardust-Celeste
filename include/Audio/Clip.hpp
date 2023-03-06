#pragma once

#include <Utilities/Types.hpp>
#include <string>

namespace Stardust_Celeste::Audio {
/**
 * @brief Clip is the foundational audio class -- it loads an audio stream from
 * a file in either static or streaming mode and then allows you to playback
 * said audio.
 *
 * Limitations:
 * WAV (Non-Streaming)
 * Mixer_LoadMUS formats (Streaming)
 *
 * It is extensible
 */
class Clip {
  public:
    /**
     * @brief Construct a new Audio Clip object
     *
     * @param path Path to the file
     * @param stream Whether the object is streaming
     */
    Clip(const std::string &&path, const bool stream = false);

    /**
     * @brief Destroy the Clip object
     *
     */
    virtual ~Clip();

    /////////////////////////////
    ///  P R O P E R T I E S  ///
    /////////////////////////////

    /**
     * @brief Set the volume value
     *
     * @param val Volume [0, 1]
     */
    auto set_volume(const float val) -> void;

    /**
     * @brief Set the clip to looping
     *
     * @param looping Whether or not the clip loops
     */
    auto set_looping(const bool looping) -> void;

    /**
     * @brief Play the clip
     *
     */
    auto play() -> void;

    /**
     * @brief Pause the audio clip (defaults to toggle behavior)
     *
     */
    auto pause() -> void;

    /**
     * @brief Stops the audio clip from playing
     *
     */
    auto stop() -> void;

  protected:
    bool isStreaming;
    bool shouldLoop;
    int chan;

    void *data; // So that we can avoid including SDL2 stuff in header
};
} // namespace Stardust_Celeste::Audio