#include "WindowFactory.h"
namespace CADMageddon
{
    std::unique_ptr<Window> CADMageddon::WindowFactory::CreateOpenGLWindow(WindowData windowData, std::shared_ptr<OpenGLManager> openGlManager)
    {
        return std::unique_ptr<Window>(new OpenGLWindow(windowData, openGlManager));
    }
}
