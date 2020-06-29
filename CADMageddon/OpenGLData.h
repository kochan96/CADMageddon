#pragma once
namespace CADMageddon
{
    struct OpenGLData
    {
        OpenGLData(int minorVersion, int majorVersion, int profile) : m_majorVersion(majorVersion), m_minorVersion(minorVersion), m_profile(profile) {};
        int m_majorVersion;
        int m_minorVersion;
        int m_profile;
    };
}