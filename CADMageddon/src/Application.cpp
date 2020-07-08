#include "Application.h"
#include "Logging/Logger.h"

namespace CADMageddon
{
    Application* Application::m_Instance = nullptr;

    Application::Application()
    {
        m_Instance = this;
        m_openGlManager = std::make_shared<OpenGLManager>();
        m_window = std::make_unique<Window>();
    }
    

    void Application::Init()
    {
        Logger::Init();
        WindowProps windowData("Hello Window");
        OpenGLData openGLData(4, 4, GLFW_OPENGL_CORE_PROFILE);

        m_window->Init(windowData, openGLData);
        m_openGlManager->Init(openGLData);
    }

    int Application::Run()
    {
        m_window->OnUpdate();

        return 0;
    }

    void Application::OnEvent(Event& e)
    {

    }
}