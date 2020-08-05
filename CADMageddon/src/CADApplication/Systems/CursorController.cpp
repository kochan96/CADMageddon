#include "CursorController.h"

#include "Core/Input.h"
#include <glm\gtc\matrix_transform.hpp>

namespace CADMageddon
{
    void CursorController::Resize(const glm::vec2& viewPortSize, FPSCamera& camera)
    {
        m_ViewPortSize = viewPortSize;
        m_Cursor.setScreenPosition(GetCursorScreenPosition(camera));
    }

    void CursorController::Update(Timestep ts, FPSCamera& camera, glm::vec2 mousePosition)
    {
        m_Cursor.setScreenPosition(mousePosition);
        m_Cursor.setPosition(GetCursorWorldPosition(camera));
    }

    glm::vec2 CursorController::GetCursorScreenPosition(FPSCamera& camera)
    {
        auto worldPosition = glm::vec4(m_Cursor.getPosition(), 1.0f);
        auto projectionPosition = camera.GetViewProjectionMatrix() * worldPosition;
        worldPosition /= worldPosition.w;
        float x = (worldPosition.x + 1.0f) / 2.0f * m_ViewPortSize.x;
        float y = (1.0f - worldPosition.y) / 2.0f * m_ViewPortSize.y;

        return glm::vec2(x, y);
    }

    glm::vec3 CursorController::GetCursorWorldPosition(FPSCamera& camera)
    {
        auto screenPosition = m_Cursor.getScreenPosition();
        auto worldPosition = m_Cursor.getPosition();

        glm::vec2 ndc;
        ndc.x = (screenPosition.x * 2.0f) / m_ViewPortSize.x - 1.0f;
        ndc.y = 1.0f - (screenPosition.y * 2.0f) / m_ViewPortSize.y;

        glm::vec4 ray_clip = glm::vec4(ndc.x, ndc.y, -1.0f, 1.0f);
        glm::mat4 invProjMat = glm::inverse(camera.GetProjectionMatrix());
        glm::vec4 eyeCoords = invProjMat * ray_clip;
        glm::vec3 eyeRay = glm::normalize(glm::vec3(eyeCoords.x, eyeCoords.y, -1.0f));

        glm::vec3 eyePosition = camera.GetViewMatrix() * glm::vec4(worldPosition, 1.0f);

        float t = eyePosition.z / eyeRay.z;
        eyePosition = eyeRay * t;
        glm::mat4 invViewMat = glm::inverse(camera.GetViewMatrix());

        worldPosition = invViewMat * glm::vec4(eyePosition, 1.0f);
        return worldPosition;
    }
}
