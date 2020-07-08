#pragma once
#include "Window/Window.h"
#include "OpenGL/OpenGLManager.h"
#include "Events\Event.h"
#include "Events\ApplicationEvent.h"

namespace CADMageddon
{
    class Application
    {
    public:
        Application();
        ~Application() {}

        void OnEvent(Event& e);

        void Init();
        void Run();
        void Close();

        static Application& Get() { return *m_Instance; }

        Window& GetMainWindow() { return *m_window; }

    private:
        bool OnWindowClose(WindowCloseEvent& e);
        bool OnWindowResize(WindowResizeEvent& e);

        static Application* m_Instance;

        bool m_Running = true;
        bool m_Minimized = false;
        float m_LastFrameTime = 0.0f;

        std::unique_ptr<Window> m_window;
        std::shared_ptr<OpenGLManager> m_openGlManager;
    };
}