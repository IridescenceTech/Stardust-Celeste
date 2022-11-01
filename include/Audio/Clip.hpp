#pragma once

#include <Utilities/Types.hpp>
#include <glm.hpp>
#include <string>

#include "AL/al.h"
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

namespace Stardust_Celeste::Audio {
/**
 * @brief Clip is the foundational audio class -- it loads an audio stream from
 * a file in either static or streaming mode and then allows you to playback
 * said audio.
 *
 * Limitations:
 * WAV (non streamed) OGG (streamed)
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
     * @brief Set the pitch value
     *
     * @param val Pitch modification [?, ?]
     */
    auto set_pitch(const float val) -> void;

    /**
     * @brief Set the volume value
     *
     * @param val Volume [0, 1]
     */
    auto set_volume(const float val) -> void;

    /**
     * @brief Set the position of the audio
     *
     * @param position 3D Position of the clip
     */
    auto set_position(const glm::vec3 &&position) -> void;

    /**
     * @brief Set the velocity of the audio
     *
     * @param velocity 3D Velocity of the clip
     */
    auto set_velocity(const glm::vec3 &&velocity) -> void;

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

    /**
     * @brief Updates a given audio clip (fetches data if streaming)
     *
     */
    auto update() -> void;

  protected:
    bool isStreaming;
    bool shouldLoop;

    auto streamData(ALuint buffer) -> bool;

    ALuint ID;

    ALuint buffers[2];
    ALuint source;
    ALenum format;

    size_t bufferSize;
    size_t totalSamplesLeft;
};
} // namespace Stardust_Celeste::Audio