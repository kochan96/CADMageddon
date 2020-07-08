#include "OpenGLManager.h"
#include "GL\glew.h"
#include "../Logging/Logger.h"

namespace CADMageddon
{
    OpenGLManager::OpenGLManager()
    {

    }

    int OpenGLManager::Init(const OpenGLData& data)
    {
        if (glewInit() != GLEW_OK)
        {

            return -1;
        }

        return 0;
    }

    void OpenGLManager::setClearColor(float r, float g, float b, float a)
    {
        glClearColor(r, g, b, a);
    }

    void OpenGLManager::setViewPort(int width, int height)
    {
        glViewport(0, 0, width, height);
    }

    void OpenGLManager::clearColorBuffer()
    {
        glClear(GL_COLOR_BUFFER_BIT);
    }

}
