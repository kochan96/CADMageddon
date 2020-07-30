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
        void InitGridVertexArray();

        void RenderMainMenuBar();
        void RenderHierarchy();
        void RenderInspector();
        void RenderViewport();

    private:
        Ref<OpenGLVertexArray> m_GridVertexArray;
        Ref<OpenGLFramebuffer> m_Framebuffer;

        FPSCameraController m_CameraController;

        glm::vec2 m_ViewportSize = { 0.0f, 0.0f };

        bool m_ViewportFocused = false;
        bool m_ViewportHovered = false;
        bool m_BlockEvents = false;


        Scene m_Scene;
        Entity m_SelectedEntity;
    };
}