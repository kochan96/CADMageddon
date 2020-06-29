#include "OpenGLWindow.h"
#include "GL\glew.h"
#include "GLFW\glfw3.h"

namespace CADMageddon
{
    OpenGLWindow::OpenGLWindow(WindowData windowData, std::shared_ptr<OpenGLManager> openGlManager) :Window(windowData), m_openGlManager(openGlManager)
    {
    }

    OpenGLWindow::~OpenGLWindow()
    {
        glfwTerminate();
    }

    int OpenGLWindow::Init()
    {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, m_openGlManager->getMajorVersion());
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, m_openGlManager->getMinorVersion());
        glfwWindowHint(GLFW_OPENGL_PROFILE, m_openGlManager->getProfile());

        m_window = glfwCreateWindow(m_windowData.m_width, m_windowData.m_height, m_windowData.m_title.c_str(), NULL, NULL);

        if (m_window == NULL)
        {
            glfwTerminate();
            return -1;
        }

        glfwMakeContextCurrent(m_window);

        return 0;
    }


    void OpenGLWindow::MainLoop()
    {
        m_openGlManager->setViewPort(800, 600);
        m_openGlManager->setClearColor(0.2f, 0.3f, 0.3f, 1.0f);

        while (!glfwWindowShouldClose(m_window))
        {

            m_openGlManager->clearColorBuffer();


            glfwSwapBuffers(m_window);
            glfwPollEvents();
        }
    }


}
