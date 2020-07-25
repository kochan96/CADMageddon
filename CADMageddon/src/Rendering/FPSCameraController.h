#pragma once
#include "Camera.h"
#include "Core\Timestep.h"

#include "Events\MouseEvent.h"
#include "Events\ApplicationEvent.h"

namespace CADMageddon
{
    class FPSCameraController
    {
    public:
        FPSCameraController(float fov, float aspectRatio, float nearClipPlane, float farClipPlane);

        void OnUpdate(Timestep ts);

        void OnEvent(Event& e);

        void OnResize(float width, float height);

        FPSCamera& GetCamera() { return m_Camera; }
        const FPSCamera& GetCamera() const { return m_Camera; }

    private:
        void HandleRotation(Timestep ts);

        bool OnMouseScrolled(MouseScrolledEvent& e);
        bool OnWindowResized(WindowResizeEvent& e);

        glm::vec3 GetCameraRight();
        glm::vec3 GetCameraUp();
        glm::vec3 GetCameraFront();

    private:
        float m_Fov;
        float m_AspectRation;
        FPSCamera m_Camera;

        glm::vec3 m_CameraPosition = { 0.0f,2.5f,5.0f };
        glm::vec2 m_CameraRotation = { 0.0f,0.0f };
        glm::vec2 lastMousePos = { 0.0f,0.0f };

        float m_CameraTranslationSpeed = 5.0f;
        float m_CameraRotationSpeed = 0.1f;
    };
}