#include "Camera.h"
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtx\rotate_vector.hpp>

namespace CADMageddon
{
    FPSCamera::FPSCamera(float fov, float aspectRatio, float nearClipPlane, float farClipPlane)
        :m_fov(fov), m_aspectRatio(aspectRatio), m_nearClipPlane(nearClipPlane),
        m_farClipPlane(farClipPlane)
    {
        m_ProjectionMatrix = glm::perspective(glm::radians(fov), aspectRatio, nearClipPlane, farClipPlane);
        m_ViewMatrix = glm::mat4(1.0f);
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
    }

    void FPSCamera::RecalculateViewMatrix()
    {
        glm::vec3 front(0.0f, 0.0f, -1.0f);
        front = glm::rotateX(front, m_Rotation.x);
        front = glm::rotateY(front, m_Rotation.y);
        front = glm::normalize(front);

        m_ViewMatrix = glm::lookAt(m_Position, m_Position + front, glm::vec3(0.0f, 1.0f, 0.0f));
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
    }

    void FPSCamera::RecalculateProjectionMatrix()
    {
        m_ProjectionMatrix = glm::perspective(glm::radians(m_fov), m_aspectRatio, m_nearClipPlane, m_farClipPlane);
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
    }
}
