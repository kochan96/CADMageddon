#include "Logger.h"
#include <spdlog/sinks/stdout_color_sinks.h>

namespace CADMageddon
{
    std::shared_ptr<spdlog::logger> Logger::m_appLogger;

    void Logger::Init()
    {
        spdlog::set_pattern("%^[%T] %n: [source %s] [function %!] [line %#]  %v%$");
        m_appLogger = spdlog::stdout_color_mt("LOG");
        m_appLogger->set_level(spdlog::level::trace);
    }
}