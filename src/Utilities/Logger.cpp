#include <Utilities/Logger.hpp>

namespace Stardust_Celeste::Utilities {

RefPtr<Logger> Logger::s_Core;
RefPtr<Logger> Logger::s_Application;

/**
 * @brief Create the static members
 */
auto Utilities::Logger::init() -> void {
#if BUILD_PLAT != BUILD_VITA
    s_Core = create_refptr<Logger>("CORE", "core_log.log");
    s_Application = create_refptr<Logger>("APP", "app_log.log");
#else
    s_Core = create_refptr<Logger>("CORE", "ux0:/data/SDC/core_log.log");
    s_Application = create_refptr<Logger>("APP", "ux0:/data/SDC/app_log.log");
#endif
}

/**
 * @brief Creates a Logger Instances
 * @param name - Name of the Logger
 * @param path - Path to file output
 */
Logger::Logger(const char *name, const char *path)
    : m_FileOut(nullptr), m_CutoffLevel(LogLevel::Trace), m_LogName(name) {
    std_output = true;
    flush_output = false;
    m_FileOut = fopen(path, "w");
}

/**
 * @brief Closes and flushes the file.
 */
Logger::~Logger() { fclose(m_FileOut); }

/**
 * @brief Cleans up the static loggers.
 */
auto Logger::cleanup() -> void {
    s_Core.reset();
    s_Application.reset();
}
} // namespace Stardust_Celeste::Utilities