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
#include "Systems\TransformationSystem.h"

#include "Panels\HierarchyPanel.h"
#include "Panels\InspectorPanel.h"

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
        bool OnMouseButtonPressed(MouseButtonPressedEvent& e);

        bool OnKeyPressedEventViewport(KeyPressedEvent& e);
        bool OnKeyPressedEvent(KeyPressedEvent& e);

        void OnSelectionChanged(bool selected, Entity entity);
        void OnSelectionCleared(std::vector<Entity> cleared);

        bool IsEditMode() const;

        void InitImGui();
        void ShutDownImGui();
        void RenderImGui();
        void InitGridVertexArray();

        void RenderMainMenuBar();
        void RenderViewport();
        void RenderOptions();
        void RenderCursor(glm::vec3 position, float size);

        glm::vec2 GetViewPortMousePosition(const glm::vec2& mousePosition);
        glm::vec2 GetNDCMousePosition(const glm::vec2& mousePosition);
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


        Ref<Scene> m_Scene;
        EditorMode m_EditorMode = EditorMode::MoveCursor;

        CursorController m_CursorController;

        Ref<PickingSystem> m_PickingSystem;
        Ref<TransformationSystem> m_TransformationSystem;

        Ref<HierarchyPanel> m_HierarchyPanel;
        Ref<InspectorPanel> m_InspectorPanel;

    };
}