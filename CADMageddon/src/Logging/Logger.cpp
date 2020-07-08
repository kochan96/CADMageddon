#include "Logger.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace CADMageddon
{
    std::shared_ptr<spdlog::logger> Logger::m_appLogger;
    std::shared_ptr<spdlog::logger> Logger::m_openglLogger;

    void Logger::Init()
    {
        spdlog::set_pattern("%^[%T] %n: %v%$");
        m_appLogger = spdlog::stdout_color_mt("App");
        m_appLogger->set_level(spdlog::level::trace);

        m_openglLogger = spdlog::stdout_color_mt("OpenGL");
        m_openglLogger->set_level(spdlog::level::trace);
    }
}