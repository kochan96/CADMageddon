#pragma once
#include <memory>
#include "WindowData.h"

namespace CADMageddon
{
    class Window
    {
    public:
        Window(WindowData windowData);
        virtual ~Window() {};
        virtual int Init() = 0;
        virtual void MainLoop() = 0;

    protected:
        WindowData m_windowData;
    };
}