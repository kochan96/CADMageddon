#include "Window.h"
#include "../Logging/Logger.h"
#include "../Input/Input.h"

#include "../Events/KeyEvent.h"
#include "../Events/MouseEvent.h"
#include "../Events/ApplicationEvent.h"


namespace CADMageddon
{
    static uint8_t m_GLFWWindowCount = 0;

    static void GLFWErrorCallback(int error, const char* description)
    {
        Logger::getOpenGlLogger()->error("GLFW Error ({0}): {1}", error, description);
    }

    Window::Window()
    {
    }

    Window::~Window()
    {
        Shutdown();
    }

    void Window::Init(const WindowProps& props, const OpenGLData& openglData)
    {
        m_Data.Title = props.m_title;
        m_Data.Width = props.m_width;
        m_Data.Height = props.m_height;

        if (m_GLFWWindowCount == 0)
        {
            int success = glfwInit();
            if (!success)
            {
                Logger::getAppLogger()->error("Could not initialize GLFW");
            }

            glfwSetErrorCallback(GLFWErrorCallback);
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, openglData.m_majorVersion);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, openglData.m_minorVersion);
        glfwWindowHint(GLFW_OPENGL_PROFILE, openglData.m_profile);
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, openglData.m_debug ? GL_TRUE : GL_FALSE);

        m_Window = glfwCreateWindow((int)m_Data.Width, (int)m_Data.Height, m_Data.Title.c_str(), nullptr, nullptr);
        ++m_GLFWWindowCount;

        glfwSetWindowUserPointer(m_Window, &m_Data);
        SetVSync(true);

        glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
            {
                WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
                data.Width = width;
                data.Height = height;

                WindowResizeEvent event(width, height);
                data.EventCallback(event);
            });

        glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
            {
                WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
                WindowCloseEvent event;
                data.EventCallback(event);
            });

        glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
            {
                WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

                switch (action)
                {
                case GLFW_PRESS:
                {
                    KeyPressedEvent event(static_cast<KeyCode>(key), 0);
                    data.EventCallback(event);
                    break;
                }
                case GLFW_RELEASE:
                {
                    KeyReleasedEvent event(static_cast<KeyCode>(key));
                    data.EventCallback(event);
                    break;
                }
                case GLFW_REPEAT:
                {
                    KeyPressedEvent event(static_cast<KeyCode>(key), 1);
                    data.EventCallback(event);
                    break;
                }
                }
            });

        glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int keycode)
            {
                WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

                KeyTypedEvent event(static_cast<KeyCode>(keycode));
                data.EventCallback(event);
            });

        glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
            {
                WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

                switch (action)
                {
                case GLFW_PRESS:
                {
                    MouseButtonPressedEvent event(static_cast<MouseCode>(button));
                    data.EventCallback(event);
                    break;
                }
                case GLFW_RELEASE:
                {
                    MouseButtonReleasedEvent event(static_cast<MouseCode>(button));
                    data.EventCallback(event);
                    break;
                }
                }
            });

        glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset)
            {
                WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

                MouseScrolledEvent event((float)xOffset, (float)yOffset);
                data.EventCallback(event);
            });

        glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos)
            {
                WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

                MouseMovedEvent event((float)xPos, (float)yPos);
                data.EventCallback(event);
            });
    }

    void Window::Shutdown()
    {

        glfwDestroyWindow(m_Window);
        --m_GLFWWindowCount;

        if (m_GLFWWindowCount == 0)
        {
            glfwTerminate();
        }
    }

    void Window::OnUpdate()
    {

        glfwPollEvents();
        glfwSwapBuffers(m_Window);
    }

    void Window::SetVSync(bool enabled)
    {
        if (enabled)
            glfwSwapInterval(1);
        else
            glfwSwapInterval(0);

        m_Data.VSync = enabled;
    }

    bool Window::IsVSync() const
    {
        return m_Data.VSync;
    }

}
