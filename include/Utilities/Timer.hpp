#pragma once
#include "NonCopy.hpp"
#include "Types.hpp"
#include <chrono>

namespace Stardust_Celeste::Utilities {

/**
 * @brief Basic object to grab time differences
 *
 */
class Timer final : public NonCopy {
  public:
    Timer();
    Timer(Timer &&other) noexcept;
    Timer &operator=(Timer &&other) noexcept;
    ~Timer();

    /**
     * @brief Initializes the global timer
     */
    static auto init() -> void;

    /**
     * @brief Cleans up the application timer.
     */
    static auto cleanup() -> void;

    /**
     * @brief Gets our main application timer
     * @return - Application Timer RefPtr
     */
    static RefPtr<Timer> &get_app_timer() { return s_Time; }

    /**
     * @brief Get the Delta Time
     * @return double time
     */
    auto get_delta_time() -> double;

    /**
     * @brief Resets the timer to zero elapsed time (no delta)
     */
    auto reset() -> void;

    /**
     * @brief Total elapsed time
     * @return double time
     */
    auto elapsed() const -> double;

  private:
    double dt;
    double total;
    std::chrono::time_point<std::chrono::high_resolution_clock> last;

    static RefPtr<Timer> s_Time;
};
} // namespace Stardust_Celeste::Utilities
