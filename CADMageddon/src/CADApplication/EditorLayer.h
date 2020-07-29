#pragma once
#include "Core\Layer.h"
#include "Rendering\VertexArray.h"
#include "Rendering\Shader.h"
#include "Rendering\FrameBuffer.h"

#include "Rendering\FPSCameraController.h"

#include "Scene\Entity.h"

namespace CADMageddon
{
    class EditorLayer : public Layer
    {
    public:
        EditorLayer(const std::string& debugName);
        void OnAttach() override;
        void OnDetach() override;
        void OnUpdate(Timestep ts) override;
        void OnEvent(Event& event) override;

    private:
        void InitImGui();
        void ShutDownImGui();
        void RenderImGui();

    private:

        Ref<OpenGLVertexArray> m_VertexArray;
        Ref<OpenGLShader> m_Shader;
        Ref<OpenGLFramebuffer> m_Framebuffer;
        Ref<OpenGLVertexBuffer> m_VertexBuffer;
        Ref<OpenGLIndexBuffer> m_IndexBuffer;

        FPSCameraController m_CameraController;

        glm::vec2 m_ViewportSize = { 0.0f, 0.0f };

        bool m_ViewportFocused = false;
        bool m_ViewportHovered = false;
        bool m_BlockEvents = false;


        Scene m_Scene;
        Entity m_Torus;
    };
}