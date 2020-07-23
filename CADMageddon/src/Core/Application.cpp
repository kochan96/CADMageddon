#include "Application.h"
#include "Logger.h"

#include "Base.h"
#include "Timestep.h"

#include "CADApplication\EditorLayer.h"

namespace CADMageddon
{
    Application* Application::m_Instance = nullptr;

    Application::Application()
    {
        m_Instance = this;
        m_window = CreateScope<Window>();
    }

    Application::~Application()
    {

    }

    void Application::Init()
    {
        Logger::Init();
        WindowProps windowData("Hello Window");

        m_window->Init(windowData);
        m_window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));

        PushLayer(new EditorLayer("EditorLayer"));
    }

    void Application::OnEvent(Event& e)
    {
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowClose));
        dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(Application::OnWindowResize));

        for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it)
        {
            if (e.Handled)
                break;
            (*it)->OnEvent(e);
        }
    }

    void Application::PushLayer(Layer* layer)
    {
        m_LayerStack.PushLayer(layer);
        layer->OnAttach();
    }

    void Application::PushOverlay(Layer* layer)
    {
        m_LayerStack.PushOverlay(layer);
        layer->OnAttach();
    }

    void Application::Run()
    {

        while (m_Running)
        {

            float time = (float)glfwGetTime();
            Timestep timestep = time - m_LastFrameTime;
            m_LastFrameTime = time;

            if (!m_Minimized)
            {
                {
                    for (Layer* layer : m_LayerStack)
                        layer->OnUpdate(timestep);
                }

                /*m_ImGuiLayer->Begin();
                {
                    HZ_PROFILE_SCOPE("LayerStack OnImGuiRender");

                    for (Layer* layer : m_LayerStack)
                        layer->OnImGuiRender();
                }
                m_ImGuiLayer->End();*/
            }

            m_window->OnUpdate();
        }
    }

    void Application::Close()
    {
        m_Running = false;
    }

    bool Application::OnWindowClose(WindowCloseEvent& e)
    {
        m_Running = false;
        return true;
    }

    bool Application::OnWindowResize(WindowResizeEvent& e)
    {

        if (e.GetWidth() == 0 || e.GetHeight() == 0)
        {
            m_Minimized = true;
            return false;
        }

        m_Minimized = false;

        glViewport(0, 0, e.GetWidth(), e.GetHeight());
        //m_openGlManager->setViewPort(e.GetWidth(), e.GetHeight());

        return false;
    }
}