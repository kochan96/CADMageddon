#pragma once
#include "cadpch.h"
#include "Core/Base.h"
#include "WindowProps.h"
#include "Events/Event.h"
#include "Rendering/Context.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>


namespace CADMageddon
{
    class Window
    {

    public:
        using EventCallbackFn = std::function<void(Event&)>;

        Window();
        virtual ~Window();

        void OnUpdate();

        unsigned int GetWidth() const { return m_Data.Width; }
        unsigned int GetHeight() const { return m_Data.Height; }

        // Window attributes
        void SetEventCallback(const EventCallbackFn& callback) { m_Data.EventCallback = callback; }
        void SetVSync(bool enabled);
        bool IsVSync() const;

        virtual void* GetNativeWindow() const { return m_Window; }

        virtual void Init(const WindowProps& props);
        virtual void Shutdown();
    
    private:
        GLFWwindow* m_Window;
        Scope<OpenGLContext> m_Context;

        struct WindowData
        {
            std::string Title;
            unsigned int Width, Height;
            bool VSync;

            EventCallbackFn EventCallback;
        };

        WindowData m_Data;
    };
}