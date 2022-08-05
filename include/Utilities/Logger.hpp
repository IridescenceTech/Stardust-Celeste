/**
 * @file Logger.hpp
 * @author Nathan Bourgeois <iridescentrosesfall@gmail.com>
 * @brief Defines the global logger object
 * @version 0.1
 * @date 2021-12-15
 *
 * @copyright Copyright (c) 2021
 *
 */

#pragma once
#include "../Platform/Platform.hpp"
#include "../fmt/core.h"
#include "Singleton.hpp"
#include "ThreadSafe.hpp"
#include "Types.hpp"

namespace Stardust_Celeste::Utilities {

/**
 * @brief Basic logging levels
 */
enum class LogLevel { Trace, Debug, Info, Warn, Error };

class Logger final : public Singleton {

  public:
    /**
     * @brief A basic logger object
     * @param name - Name of logger
     * @param path - Output file path
     */
    Logger(const char *name, const char *path);

    /**
     * @brief Destructor.
     */
    ~Logger();

    /**
     * @brief Initializes the core and app loggers
     */
    static auto init() -> void;

    /**
     * @brief Cleans up the core and app loggers (calls delete)
     */
    static auto cleanup() -> void;

    /**
     * @brief Gets the Core Logger
     * @return - RefPtr to the Core Logger
     */
    static RefPtr<Logger> &get_core_log() { return s_Core; }

    /**
     * @brief Gets the App Logger
     * @return  - RefPtr to the App Logger
     */
    static RefPtr<Logger> &get_app_log() { return s_Application; }

    /**
     * @brief Logs a message to a logger object. Will not log if the message is
     * below the cutoff level. Locks itself to have threadsafety.
     * @param msg - Message to send
     * @param lvl - The Level of the message
     * @param args - The arguments for formatting
     */
    template <typename... Args>
    auto log(const std::string &msg, const LogLevel &lvl, Args &&...args)
        -> void {

        std::lock_guard guard(writeLock);

        if (lvl >= m_CutoffLevel) {
            auto formatStr = ("[" + std::string(m_LogName) + "]") + "[" +
                             logLevelToString(lvl) + "]: ";
            auto formatMsg = fmt::format(formatStr + msg + "\n",
                                         std::forward<Args>(args)...);

#if BUILD_PLAT != BUILD_VITA
            if (std_output)
                fmt::print(formatMsg);
#endif

            fmt::print(m_FileOut, formatMsg);

            if (flush_output)
                fflush(m_FileOut);
        }
    }

    /**
     * @brief Sets the cutoff level
     * @param lvl - Level to cutoff past.
     */
    inline auto setCutoff(const LogLevel &lvl) const -> void {
        m_CutoffLevel = lvl;
    }

    /**
     * @brief Utility functions for a specific level
     */
    template <typename... Args>
    inline auto trace(const std::string &msg, Args &...args) -> void {
        log(msg, LogLevel::Trace, std::forward<Args>(args)...);
    }

    template <typename... Args>
    inline auto debug(const std::string &msg, Args &...args) -> void {
        log(msg, LogLevel::Debug, std::forward<Args>(args)...);
    }

    template <typename... Args>
    inline auto info(const std::string &msg, Args &...args) -> void {
        log(msg, LogLevel::Info, std::forward<Args>(args)...);
    }

    template <typename... Args>
    inline auto warn(const std::string &msg, Args &...args) -> void {
        log(msg, LogLevel::Warn, std::forward<Args>(args)...);
    }

    template <typename... Args>
    inline auto error(const std::string &msg, Args &...args) -> void {
        log(msg, LogLevel::Error, std::forward<Args>(args)...);
    }

    bool std_output;
    bool flush_output;

  private:
    mutable LogLevel m_CutoffLevel;
    FILE *m_FileOut;
    const char *m_LogName;

    static RefPtr<Logger> s_Core;
    static RefPtr<Logger> s_Application;

    std::mutex writeLock;

    /**
     * @brief Converts LogLevel to const char*
     * @param level - level
     * @return - Return a string of the name
     */
    static inline auto logLevelToString(const LogLevel &level) -> const char * {
        switch (level) {
        case LogLevel::Trace:
            return "TRACE";

        case LogLevel::Debug:
            return "DEBUG";

        case LogLevel::Info:
            return "INFO";

        case LogLevel::Warn:
            return "WARN";

        case LogLevel::Error:
            return "ERROR";
        }

        return "?";
    }
};
} // namespace Stardust_Celeste::Utilities

/**
 * Macros to use the core logger.
 */
#define SC_CORE_TRACE(...)                                                     \
    Stardust_Celeste::Utilities::Logger::get_core_log()->trace(__VA_ARGS__)
#define SC_CORE_DEBUG(...)                                                     \
    Stardust_Celeste::Utilities::Logger::get_core_log()->debug(__VA_ARGS__)
#define SC_CORE_INFO(...)                                                      \
    Stardust_Celeste::Utilities::Logger::get_core_log()->info(__VA_ARGS__)
#define SC_CORE_WARN(...)                                                      \
    Stardust_Celeste::Utilities::Logger::get_core_log()->warn(__VA_ARGS__)
#define SC_CORE_ERROR(...)                                                     \
    Stardust_Celeste::Utilities::Logger::get_core_log()->error(__VA_ARGS__)

/**
 * Macros to use the application logger.
 */
#define SC_APP_TRACE(...)                                                      \
    Stardust_Celeste::Utilities::Logger::get_app_log()->trace(__VA_ARGS__)
#define SC_APP_DEBUG(...)                                                      \
    Stardust_Celeste::Utilities::Logger::get_app_log()->debug(__VA_ARGS__)
#define SC_APP_INFO(...)                                                       \
    Stardust_Celeste::Utilities::Logger::get_app_log()->info(__VA_ARGS__)
#define SC_APP_WARN(...)                                                       \
    Stardust_Celeste::Utilities::Logger::get_app_log()->warn(__VA_ARGS__)
#define SC_APP_ERROR(...)                                                      \
    Stardust_Celeste::Utilities::Logger::get_app_log()->error(__VA_ARGS__)
