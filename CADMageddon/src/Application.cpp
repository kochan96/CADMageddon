#include "Application.h"
#include "Logging/Logger.h"

#include "Core\Macros.h"
#include "Core\Timestep.h"

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
        m_window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));
        m_openGlManager->Init(openGLData);


		m_openGlManager->setClearColor(1.0f, 0.0f, 0.0f, 1.0f);
    }

    void Application::OnEvent(Event& e)
    {
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowClose));
        dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(Application::OnWindowResize));

        /*for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it)
        {
            if (e.Handled)
                break;
            (*it)->OnEvent(e);
        }*/
    }

	void Application::Run()
	{

		while (m_Running)
		{

			float time = (float)glfwGetTime();
			Timestep timestep = time - m_LastFrameTime;
			m_LastFrameTime = time;

			m_openGlManager->clearColorBuffer();

			/*if (!m_Minimized)
			{
				{
					HZ_PROFILE_SCOPE("LayerStack OnUpdate");

					for (Layer* layer : m_LayerStack)
						layer->OnUpdate(timestep);
				}

				m_ImGuiLayer->Begin();
				{
					HZ_PROFILE_SCOPE("LayerStack OnImGuiRender");

					for (Layer* layer : m_LayerStack)
						layer->OnImGuiRender();
				}
				m_ImGuiLayer->End();
			}*/

			m_window->OnUpdate();
		}
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
		m_openGlManager->setViewPort(e.GetWidth(), e.GetHeight());

		return false;
	}
}