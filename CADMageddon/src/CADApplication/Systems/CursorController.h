#pragma once
#include "Core\Timestep.h"
#include "Cursor3D.h"
#include "Rendering\Camera.h"

namespace CADMageddon
{
    class CursorController
    {
    public:
        CursorController(FPSCamera& camera);

        void Resize(const glm::vec2& viewPortSize, const std::pair<glm::vec2, glm::vec2>& m_Viewport, FPSCamera& camera);

        void Update(Timestep ts, FPSCamera& camera, glm::vec2 mousePosition);

        const Cursor3D getCursor() const { return *m_Cursor; }

        Ref<Cursor3D> getCursor() { return m_Cursor; }

        void UpdateScreenPosition(const glm::vec2& screenPosition);
        void UpdateWorldPosition(const glm::vec3& worldPosition);

    private:
        glm::vec2 GetCursorScreenPosition(FPSCamera& camera);
        glm::vec3 GetCursorWorldPosition(FPSCamera& camera);

        glm::vec2 GetCursorViewPortPosition(const glm::vec2& screenPosition);

    private:
        FPSCamera& m_Camera;
        std::pair<glm::vec2, glm::vec2> m_Viewport;
        glm::vec2 m_ViewPortSize = { 0.0f,0.0f };
        Ref<Cursor3D> m_Cursor;
    };
}