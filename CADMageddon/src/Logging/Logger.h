#pragma once
#include <memory>
#include <spdlog\spdlog.h>

namespace CADMageddon
{
    class Logger
    {
    public:
        static void Init();

        inline static std::shared_ptr<spdlog::logger>& getOpenGlLogger() { return m_openglLogger; }
        inline static std::shared_ptr<spdlog::logger>& getAppLogger() { return m_appLogger; }

    private:
        static std::shared_ptr<spdlog::logger> m_openglLogger;
        static std::shared_ptr<spdlog::logger> m_appLogger;
    };
}