#include "Application.h"

namespace CADMageddon
{
    Application::Application()
    {
        WindowFactory windowFactory;
        WindowData windowData("Hello Window");
        OpenGLData openGLData(4, 4, GLFW_OPENGL_CORE_PROFILE);
        m_openGlManager = std::make_shared<OpenGLManager>(openGLData);
        m_window = windowFactory.CreateOpenGLWindow(windowData, m_openGlManager);
    }

    void Application::Init()
    {
        m_window->Init();
        m_openGlManager->Init();
    }

    int Application::Run()
    {
        m_window->MainLoop();

        return 0;
    }
}