#include "cadpch.h"
#include "Context.h"

#include <glad\glad.h>
#include <GLFW\glfw3.h>

namespace CADMageddon
{
    OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
        :m_windowHandle(windowHandle)
    {
    }

    void OpenGLContext::Init()
    {
        glfwMakeContextCurrent(m_windowHandle);
        int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress); //TODO error checking


        LOG_INFO("OpenGL Info:");
        LOG_INFO("  Vendor: {0}", glGetString(GL_VENDOR));
        LOG_INFO("  Renderer: {0}", glGetString(GL_RENDERER));
        LOG_INFO("  Version: {0}", glGetString(GL_VERSION));

        int versionMajor;
        int versionMinor;
        glGetIntegerv(GL_MAJOR_VERSION, &versionMajor);
        glGetIntegerv(GL_MINOR_VERSION, &versionMinor);
    }

    void OpenGLContext::SwapBuffers()
    {
        glfwSwapBuffers(m_windowHandle);
    }
}
