#pragma once
namespace CADMageddon
{
    struct OpenGLData
    {
        OpenGLData(int minorVersion = 4, int majorVersion = 4, int profile = 0x00032001, bool debug = true)
            : m_majorVersion(majorVersion), m_minorVersion(minorVersion), m_profile(profile), m_debug(debug) {};
        int m_majorVersion;
        int m_minorVersion;
        int m_profile;
        bool m_debug;
    };
}