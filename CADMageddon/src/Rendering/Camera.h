#pragma once
#include <glm\glm.hpp>


namespace CADMageddon
{
    class FPSCamera
    {
    public:
        FPSCamera(float fov, float aspectRatio, float nearClipPlane, float farClipPlane);
        ~FPSCamera() {}

        float GetAspectRatio() const { return m_aspectRatio; }
        void SetAspectRatio(float aspectRatio) { m_aspectRatio = aspectRatio; RecalculateProjectionMatrix(); };

        float GetNearPlane() const { return m_nearClipPlane; }

        float GetFieldOfView() const { return m_fov; }
        void SetFieldOfView(float fov) { m_fov = fov; RecalculateProjectionMatrix(); }

        const glm::vec3& GetPosition() const { return m_Position; }
        void SetPosition(const glm::vec3& position) { m_Position = position; RecalculateViewMatrix(); }

        const glm::vec2& GetRotation() const { return m_Rotation; }
        void SetRotation(const glm::vec2& rotation) { m_Rotation = rotation; RecalculateViewMatrix(); }

        const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
        const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
        const glm::mat4& GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }

    private:
        void RecalculateViewMatrix();
        void RecalculateProjectionMatrix();

    private:
        glm::mat4 m_ProjectionMatrix;
        glm::mat4 m_ViewMatrix;
        glm::mat4 m_ViewProjectionMatrix;

        glm::vec3 m_Position = { 0.0f,0.0f,0.0f };
        glm::vec2 m_Rotation = { 0.0f,0.0f };

        float m_fov;
        float m_aspectRatio;
        float m_nearClipPlane;
        float m_farClipPlane;
    };

}