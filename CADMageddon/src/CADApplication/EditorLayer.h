#pragma once
#include "Core\Layer.h"
#include "Events\KeyEvent.h"
#include "Rendering\VertexArray.h"
#include "Rendering\Shader.h"
#include "Rendering\FrameBuffer.h"

#include "Rendering\FPSCameraController.h"

#include "Systems\PickingSystem.h"
#include "Systems\CursorController.h"
#include "Systems\TransformationSystem.h"

#include "CADApplication/Panels/HierarchyPanel.h"
#include "CADApplication/Panels/InspectorPanel.h"

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

        void OnSelectionChangedPoint(bool selected, Ref<Point> point);
        void OnSelectionChangedTorus(bool selected, Ref<Torus> torus);
        void OnSelectionChangedBezierC0(bool selected, Ref<BezierC0> bezier);
        void OnSelectionChangedBSpline(bool selected, Ref<BSpline> bSpline);
        void OnSelectionInterpolatedChanged(bool selected, Ref<InterpolatedCurve> interpolated);
        void OnSelectionChangedBezierPatch(bool selected, Ref<BezierPatch> bezierPatch);
        void OnSelectionChangedBSplinePatch(bool selected, Ref<BSplinePatch> bezierPatch);
        void OnPickingSelectionCleared();
        void OnSelectionCleared();

        bool IsEditMode() const;

        void InitImGui();
        void ShutDownImGui();
        void RenderImGui();
        void InitGridVertexArray();
        void InitQuadVertexArray();
        void InitQuadShader();

        void RenderMainMenuBar();
        void RenderViewport();
        void RenderOptions();
        void RenderCursor(glm::vec3 position, float size, const glm::vec4& color = glm::vec4(1.0f));

        glm::vec2 GetViewPortMousePosition(const glm::vec2& mousePosition);
        glm::vec2 GetNDCMousePosition(const glm::vec2& mousePosition);
        bool IsMouseInsideViewPort();

    private:
        Ref<OpenGLVertexArray> m_GridVertexArray;
        Ref<OpenGLVertexArray> m_QuadVertexArray;
        Ref<OpenGLShader> m_QuadShader;

        Ref<OpenGLFramebuffer> m_Framebuffer;
        Ref<OpenGLFramebuffer> m_FramebufferLeft;
        Ref<OpenGLFramebuffer> m_FramebufferRight;

        FPSCameraController m_CameraController;

        glm::vec2 m_ViewportSize = { 0.0f, 0.0f };
        std::pair<glm::vec2, glm::vec2> m_Viewport;
        glm::vec2 m_MousePosition = { 0.0f,0.0f };

        bool m_ViewportFocused = false;
        bool m_ViewportHovered = false;
        bool m_BlockEvents = false;

        bool m_ShowBezierPatchRectCreationPopup = false;
        bool m_ShowBezierPatchCylinderCreationPopup = false;

        bool m_ShowBSplinePatchRectCreationPopup = false;
        bool m_ShowBSplinePatchCylinderCreationPopup = false;

        PatchRectCreationParameters m_PatchRectCreationParameters;
        PatchCylinderCreationParameters m_PatchCylinderCreationParameters;

        bool m_EnableStereoscopic = false;
        glm::vec4 m_LeftEyeColor = glm::vec4(1, 0, 0, 1.0f);
        glm::vec4 m_RightEyeColor = glm::vec4(0, 0, 1, 1.0f);

        bool m_ShowGrid = true;

        Ref<Scene> m_Scene;
        EditorMode m_EditorMode = EditorMode::MoveCursor;

        CursorController m_CursorController;

        Ref<PickingSystem> m_PickingSystem;
        Ref<TransformationSystem> m_TransformationSystem;

        Ref<HierarchyPanel> m_HierarchyPanel;
        Ref<InspectorPanel> m_InspectorPanel;

    };
}