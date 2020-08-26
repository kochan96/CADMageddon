#include "Camera.h"
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtx\rotate_vector.hpp>

namespace CADMageddon
{
    FPSCamera::FPSCamera(float fov, float aspectRatio, float nearClipPlane, float farClipPlane)
        :m_fov(fov), m_aspectRatio(aspectRatio), m_nearClipPlane(nearClipPlane),
        m_farClipPlane(farClipPlane)
    {
        m_ViewMatrix = glm::mat4(1.0f);
        RecalculateProjectionMatrix();
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

        RecalculateStereoViewMatrix();

    }

    void FPSCamera::RecalculateProjectionMatrix()
    {
        m_ProjectionMatrix = glm::perspective(glm::radians(m_fov), m_aspectRatio, m_nearClipPlane, m_farClipPlane);
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;

        RecalculateStereoProjectionMatrix();
    }

    void FPSCamera::RecalculateStereoViewMatrix()
    {
        glm::vec3 front(0.0f, 0.0f, -1.0f);
        front = glm::rotateX(front, m_Rotation.x);
        front = glm::rotateY(front, m_Rotation.y);
        front = glm::normalize(front);

        glm::vec3 right(1.0f, 0.0f, 0.0f);
        right = glm::rotateX(right, m_Rotation.x);
        right = glm::rotateY(right, m_Rotation.y);
        right = glm::normalize(right);

        float eyeDistance = m_EyeDistance / 2.0f;

        m_LeftViewMatrix = glm::lookAt(m_Position + right * eyeDistance, m_Position + front + right * eyeDistance, glm::vec3(0.0f, 1.0f, 0.0f));
        m_RightViewMatrix = glm::lookAt(m_Position + right * -eyeDistance, m_Position + front + right * -eyeDistance, glm::vec3(0.0f, 1.0f, 0.0f));

    }

    //https://cgvr.cs.uni-bremen.de/teaching/vr_literatur/Rendering%203D%20Anaglyph%20in%20OpenGL.pdf?fbclid=IwAR2K8EkzIy6kP5HDrJgYVr7lh4rENgbXysNibTHDmNotnSKxUKhQcopUxks
    void FPSCamera::RecalculateStereoProjectionMatrix()
    {
        float eyeDistance = m_EyeDistance / 2.0f;

        float top = m_nearClipPlane * std::tanf(glm::radians(m_fov) / 2);
        float bottom = -top;
        float a = m_aspectRatio * std::tanf(glm::radians(m_fov) / 2) * m_ProjectionPlaneDistance;

        float b = a - eyeDistance;
        float c = a + eyeDistance;

        float left = -b * m_nearClipPlane / m_ProjectionPlaneDistance;
        float right = c * m_nearClipPlane / m_ProjectionPlaneDistance;

        m_LeftEyeProjectionMatrix = glm::frustum(left, right, bottom, top, m_nearClipPlane, m_farClipPlane);

        top = m_nearClipPlane * std::tanf(glm::radians(m_fov) / 2);
        bottom = -top;
        a = m_aspectRatio * std::tanf(glm::radians(m_fov) / 2) * m_ProjectionPlaneDistance;

        b = a - eyeDistance;
        c = a + eyeDistance;

        left = -c * m_nearClipPlane / m_ProjectionPlaneDistance;
        right = b * m_nearClipPlane / m_ProjectionPlaneDistance;
        m_RightEyeProjectionMatrix = glm::frustum(left, right, bottom, top, m_nearClipPlane, m_farClipPlane);
    }
}
