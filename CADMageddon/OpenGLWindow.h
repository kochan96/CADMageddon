#pragma once
#include <memory>
#include "Window.h"
#include "OpenGLManager.h"

#include <GL\glew.h>
#include <GLFW\glfw3.h>


namespace CADMageddon
{
    class OpenGLWindow : public Window
    {
    public:
        OpenGLWindow(WindowData windowData, std::shared_ptr<OpenGLManager> openGlManager);
        ~OpenGLWindow();
        virtual int Init() override;
        virtual void MainLoop() override;

    private:
        WindowData m_windowData;
        std::shared_ptr<OpenGLManager> m_openGlManager;
        GLFWwindow* m_window;

    };

}