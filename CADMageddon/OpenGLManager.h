#pragma once
#include "OpenGLData.h"

namespace CADMageddon
{
    class OpenGLManager
    {
    public:
        OpenGLManager(OpenGLData openGLData);
        int Init();

        int getMinorVersion() { return m_openGlData.m_minorVersion; }
        int getMajorVersion() { return m_openGlData.m_majorVersion; }
        int getProfile() { return m_openGlData.m_profile; }

        void setClearColor(float r, float g, float b, float a);

        void setViewPort(int width, int height);

        void clearColorBuffer();

    private:
        OpenGLData m_openGlData;
    };
}