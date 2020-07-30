#pragma once
#include <memory>
#include <spdlog\spdlog.h>

namespace CADMageddon
{


    class Logger
    {
    public:
        static void Init();
        inline static std::shared_ptr<spdlog::logger>& getAppLogger() { return m_appLogger; }

    private:
        static std::shared_ptr<spdlog::logger> m_appLogger;
    };


#define LOG_TRACE(...)     SPDLOG_LOGGER_TRACE(Logger::getAppLogger(),__VA_ARGS__)
#define LOG_INFO(...)      SPDLOG_LOGGER_INFO(Logger::getAppLogger(),__VA_ARGS__)
#define LOG_WARNING(...)   SPDLOG_LOGGER_WARN(Logger::getAppLogger(),__VA_ARGS__)
#define LOG_ERROR(...)     SPDLOG_LOGGER_ERROR(Logger::getAppLogger(),__VA_ARGS__)
#define LOG_CRITITCAL(...) SPDLOG_LOGGER_CRITICAL(Logger::getAppLogger(),__VA_ARGS__)

#define LOG_ASSERT(x,...) if(!x) LOG_ERROR(__VA_ARGS__)

}