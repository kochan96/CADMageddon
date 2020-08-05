#pragma once
#include "Core\Layer.h"
#include "Events\KeyEvent.h"
#include "Rendering\VertexArray.h"
#include "Rendering\Shader.h"
#include "Rendering\FrameBuffer.h"

#include "Rendering\FPSCameraController.h"
#include "Scene\Entity.h"

#include "Systems\PickingSystem.h"
#include "Systems\CursorController.h"

namespace CADMageddon
{
    enum class EditorMode
    {
        MoveCursor,
        Selection,
        Translation,
        Rotation,
        Scale
    };

    class EditorLayer : public Layer
    {
    public:
        EditorLayer(const std::string& debugName);
        void OnAttach() override;
        void OnDetach() override;
        void OnUpdate(Timestep ts) override;
        void OnEvent(Event& event) override;

    private:
        bool OnKeyPressedEvent(KeyPressedEvent& e);

        void ClearSelection();
        void HandleSingleSelection(Entity& entity, HierarchyComponent& hierarchyComponent);
        void HandleMultiSelection(Entity& entity, HierarchyComponent& hierarchyComponent);

        void InitImGui();
        void ShutDownImGui();
        void RenderImGui();
        void InitGridVertexArray();

        void RenderMainMenuBar();
        void RenderHierarchy();
        void RenderInspector();
        void RenderViewport();
        void RenderCursor(float size);

        glm::vec2 GetViewPortMousePosition(const glm::vec2& mousePosition);
        bool IsMouseInsideViewPort();

    private:
        Ref<OpenGLVertexArray> m_GridVertexArray;
        Ref<OpenGLFramebuffer> m_Framebuffer;

        FPSCameraController m_CameraController;

        glm::vec2 m_ViewportSize = { 0.0f, 0.0f };
        std::pair<glm::vec2, glm::vec2> m_Viewport;
        glm::vec2 m_MousePosition = { 0.0f,0.0f };

        bool m_ViewportFocused = false;
        bool m_ViewportHovered = false;
        bool m_BlockEvents = false;


        std::unique_ptr<Scene> m_Scene;
        Entity m_SelectedEntity;
        EditorMode m_EditorMode = EditorMode::MoveCursor;

        PickingSystem m_PickingSystem;
        CursorController m_CursorController;
    };
}