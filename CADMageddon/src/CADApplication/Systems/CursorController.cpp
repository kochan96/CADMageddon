#include "cadpch.h"
#include "CursorController.h"
#include "Core/Input.h"
#include <glm\gtc\matrix_transform.hpp>

namespace CADMageddon
{
    CursorController::CursorController(FPSCamera& camera)
        :m_Camera(camera)
    {
        m_Cursor = CreateRef<Cursor3D>();
    }

    void CursorController::Resize(const glm::vec2& viewPortSize, const std::pair<glm::vec2, glm::vec2>& viewPort, FPSCamera& camera)
    {
        m_ViewPortSize = viewPortSize;
        m_Viewport = viewPort;
        m_Cursor->setScreenPosition(GetCursorScreenPosition(camera));
    }

    void CursorController::Update(Timestep ts, FPSCamera& camera, glm::vec2 mousePosition)
    {
        UpdateScreenPosition(mousePosition);
    }

    void CursorController::UpdateScreenPosition(const glm::vec2& screenPosition)
    {
        m_Cursor->setScreenPosition(screenPosition);
        m_Cursor->setPosition(GetCursorWorldPosition(m_Camera));
    }

    void CursorController::UpdateWorldPosition(const glm::vec3& worldPosition)
    {
        m_Cursor->setPosition(worldPosition);
        m_Cursor->setScreenPosition(GetCursorScreenPosition(m_Camera));
    }

    glm::vec2 CursorController::GetCursorScreenPosition(FPSCamera& camera)
    {
        auto worldPosition = glm::vec4(m_Cursor->getPosition(), 1.0f);
        auto projectionPosition = camera.GetViewProjectionMatrix() * worldPosition;
        projectionPosition /= projectionPosition.w;
        float x = (projectionPosition.x + 1.0f) / 2.0f * m_ViewPortSize.x;
        float y = (1.0f - projectionPosition.y) / 2.0f * m_ViewPortSize.y;

        return GetCursorViewPortPosition({ x,y });
    }

    glm::vec3 CursorController::GetCursorWorldPosition(FPSCamera& camera)
    {
        auto screenPosition = m_Cursor->getScreenPosition();
        auto worldPosition = m_Cursor->getPosition();

        glm::vec2 ndc;
        ndc.x = (screenPosition.x * 2.0f) / m_ViewPortSize.x - 1.0f;
        ndc.y = 1.0f - (screenPosition.y * 2.0f) / m_ViewPortSize.y;

        glm::vec4 ray_clip = glm::vec4(ndc.x, ndc.y, -1.0f, 1.0f);
        glm::mat4 invProjMat = glm::inverse(camera.GetProjectionMatrix());
        glm::vec4 eyeCoords = invProjMat * ray_clip;
        glm::vec3 eyeRay = glm::normalize(glm::vec3(eyeCoords.x, eyeCoords.y, -1.0f));

        glm::vec3 eyePosition = camera.GetViewMatrix() * glm::vec4(worldPosition, 1.0f);

        float zPosition = std::min(eyePosition.z, camera.GetNearPlane() - 1.0f);

        float t = zPosition / eyeRay.z;
        eyePosition = eyeRay * t;
        glm::mat4 invViewMat = glm::inverse(camera.GetViewMatrix());

        worldPosition = invViewMat * glm::vec4(eyePosition, 1.0f);
        return worldPosition;
    }

    glm::vec2 CursorController::GetCursorViewPortPosition(const glm::vec2& mousePosition)
    {
        float x = mousePosition.x - m_Viewport.first.x;
        float y = mousePosition.y - m_Viewport.first.y;

        //return glm::vec2(x, y);

        return mousePosition;
    }
}
