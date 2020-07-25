#pragma once
#include <string>

namespace CADMageddon
{
    struct WindowProps
    {
        WindowProps(std::string title = "Lear OpenGl", int width = 1280, int height = 780)
            :m_title(title), m_width(width), m_height(height) {}

        std::string m_title;
        unsigned int m_width;
        unsigned int m_height;
    };
}