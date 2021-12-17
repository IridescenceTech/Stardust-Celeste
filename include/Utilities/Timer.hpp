/**
 * @file Timer.hpp
 * @author Nathan Bourgeois <iridescentrosesfall@gmail.com>
 * @brief Adds a timer
 * @version 0.1
 * @date 2021-12-15
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#pragma once
#include "NonCopy.hpp"
#include "Types.hpp"
#include <chrono>

namespace Stardust_Celeste::Utilities {

    class Timer final: public NonCopy {
    public:

        Timer();
        Timer(Timer&& other) noexcept;
        Timer& operator=(Timer&& other) noexcept;
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
        static RefPtr<Timer>& GetAppTimer() {return s_Time;}

        /**
         * @brief Get the Delta Time
         * @return double time
         */
        auto getDeltaTime() -> double;

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
}
