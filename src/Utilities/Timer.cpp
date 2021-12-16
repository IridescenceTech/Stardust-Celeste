/**
 * Timer class
 */
#include <Utilities/Timer.hpp>

namespace Stardust_Celeste::Utilities {

    RefPtr <Timer> Timer::s_Time;

    /**
     * Create a blank timer at zero.
     */
    Timer::Timer() {
        last = std::chrono::high_resolution_clock::now();
        total = 0;
        dt = 0;
    }

    /**
     * Move constructor
     * @param other - Object to move into this
     */
    Timer::Timer(Timer &&other) noexcept {
        last = other.last;
        dt = other.dt;
        total = other.total;

        other.last = std::chrono::high_resolution_clock::now();
        other.total = 0;
        other.dt = 0;
    }

    /**
     * Assignment move operator
     * @param other - Object to move into this
     * @return - This
     */
    Timer &Timer::operator=(Timer &&other) noexcept {
        last = other.last;
        dt = other.dt;
        total = other.total;

        other.last = std::chrono::high_resolution_clock::now();
        other.total = 0;
        other.dt = 0;

        return *this;
    }

    /**
     * Delete properties
     */
    Timer::~Timer() {
        total = 0;
        dt = 0;
    }

    /**
     * Initialize the static members
     */
    auto Timer::init() -> void {
        s_Time = createRefPtr<Timer>();
    }

    /**
     * Figure out time delta
     * @return - Time Delta
     */
    auto Timer::getDeltaTime() -> double {
        std::chrono::time_point<std::chrono::high_resolution_clock> current = std::chrono::high_resolution_clock::now();
        dt = std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1>>>(current - last).count();
        last = current;

        total += dt;
        return dt;
    }

    /**
     * Reset time elapsed to 0 and current time to now.
     */
    auto Timer::reset() -> void {
        last = std::chrono::high_resolution_clock::now();
        total = 0;
    }

    /**
     * Return total time
     * @return - Total elapsed time.
     */
    auto Timer::elapsed() const -> double {
        return total;
    }

    /**
     * Resets the time
     */
    auto Timer::cleanup() -> void {
        s_Time->reset();
    }
}