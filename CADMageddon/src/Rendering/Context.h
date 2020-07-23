#pragma once

struct GLFWwindow;

namespace CADMageddon
{
    class OpenGLContext
    {
    public:
        OpenGLContext(GLFWwindow* windowHandle);
        void Init();
        void SwapBuffers();

    private:
        GLFWwindow* m_windowHandle;
    };
}