#pragma once
#include <memory>
#include "OpenGLWindow.h"

namespace CADMageddon
{
    class WindowFactory
    {
    public:
        std::unique_ptr<Window> CreateOpenGLWindow(WindowData windowData, std::shared_ptr<OpenGLManager> openGlManager);
    };
}