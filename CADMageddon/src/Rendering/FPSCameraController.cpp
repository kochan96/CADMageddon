#include "cadpch.h"
#include "FPSCameraController.h"

#include "Core/Input.h"
#include "Core/KeyCodes.h"

#include "Core\Base.h"

#include <glm\gtx\rotate_vector.hpp>

namespace CADMageddon
{
    FPSCameraController::FPSCameraController(float fov, float aspectRatio, float nearClipPlane, float farClipPlane)
        :m_AspectRation(aspectRatio), m_Fov(fov), m_Camera(fov, aspectRatio, nearClipPlane, farClipPlane)
    {
    }


    glm::vec3 FPSCameraController::GetCameraRight()
    {
        glm::vec3 right(1.0f, 0.0f, 0.0f);
        right = glm::rotateX(right, m_CameraRotation.x);
        right = glm::rotateY(right, m_CameraRotation.y);

        return right;
    }

    glm::vec3 FPSCameraController::GetCameraFront()
    {
        glm::vec3 front(0.0f, 0.0f, -1.0f);
        front = glm::rotateX(front, m_CameraRotation.x);
        front = glm::rotateY(front, m_CameraRotation.y);

        return front;
    }

    glm::vec3 FPSCameraController::GetCameraUp()
    {
        glm::vec3 up(0.0f, 1.0f, 0.0f);
        up = glm::rotateX(up, m_CameraRotation.x);
        up = glm::rotateY(up, m_CameraRotation.y);

        return up;
    }

    void FPSCameraController::OnUpdate(Timestep ts)
    {

        if (Input::IsKeyPressed(CDM_KEY_A))
        {
            m_CameraPosition -= GetCameraRight() * (m_CameraTranslationSpeed * ts);
        }
        else if (Input::IsKeyPressed(CDM_KEY_D))
        {
            m_CameraPosition += GetCameraRight() * (m_CameraTranslationSpeed * ts);
        }

        if (Input::IsKeyPressed(CDM_KEY_W))
        {
            m_CameraPosition += GetCameraFront() * (m_CameraTranslationSpeed * ts);
        }
        else if (Input::IsKeyPressed(CDM_KEY_S))
        {
            m_CameraPosition -= GetCameraFront() * (m_CameraTranslationSpeed * ts);
        }

        if (Input::IsKeyPressed(CDM_KEY_Q))
        {
            m_CameraPosition += GetCameraUp() * (m_CameraTranslationSpeed * ts);
        }
        else if (Input::IsKeyPressed(CDM_KEY_E))
        {
            m_CameraPosition -= GetCameraUp() * (m_CameraTranslationSpeed * ts);
        }

        m_Camera.SetPosition(m_CameraPosition);

        HandleRotation(ts);
    }

    void FPSCameraController::HandleRotation(Timestep ts)
    {
        auto [mousePosX, mousePosY] = Input::GetMousePosition();

        if (Input::IsMouseButtonPressed(CDM_MOUSE_BUTTON_LEFT))
        {
            m_CameraRotation.x -= (mousePosY - lastMousePos.y) * m_CameraRotationSpeed * ts;
            m_CameraRotation.y -= (mousePosX - lastMousePos.x) * m_CameraRotationSpeed * ts;

            if (m_CameraRotation.x > 89.0f)
                m_CameraRotation.x = 89.0f;
            if (m_CameraRotation.x < -89.0f)
                m_CameraRotation.x = -89.0f;

            m_Camera.SetRotation(m_CameraRotation);
        }

        lastMousePos = glm::vec2(mousePosX, mousePosY);
    }

    void FPSCameraController::OnResize(float width, float height)
    {
        m_Camera.SetAspectRatio(width / height);
    }

    void FPSCameraController::OnEvent(Event& e)
    {
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<MouseScrolledEvent>(BIND_EVENT_FN(FPSCameraController::OnMouseScrolled));
        dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(FPSCameraController::OnWindowResized));
    }

    bool FPSCameraController::OnMouseScrolled(MouseScrolledEvent& e)
    {
        m_Fov -= (float)e.GetYOffset();
        if (m_Fov < 1.0f)
            m_Fov = 1.0f;
        if (m_Fov > 120.0f)
            m_Fov = 120.0f;

        m_Camera.SetFieldOfView(m_Fov);

        return false;
    }

    bool FPSCameraController::OnWindowResized(WindowResizeEvent& e)
    {
        //HZ_PROFILE_FUNCTION();

        OnResize((float)e.GetWidth(), (float)e.GetHeight());
        return false;
    }
}
