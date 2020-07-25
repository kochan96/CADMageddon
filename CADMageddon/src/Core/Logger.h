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


#define LOG_TRACE(...) Logger::getAppLogger()->trace(__VA_ARGS__)
#define LOG_INFO(...) Logger::getAppLogger()->info(__VA_ARGS__)
#define LOG_WARNING(...) Logger::getAppLogger()->warn(__VA_ARGS__)
#define LOG_ERROR(...) Logger::getAppLogger()->error(__VA_ARGS__)
#define LOG_CRITITCAL(...) Logger::getAppLogger()->critical(__VA_ARGS__)
    
}