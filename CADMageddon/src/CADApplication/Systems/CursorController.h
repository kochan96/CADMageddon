#pragma once
#include "Core\Timestep.h"
#include "Cursor3D.h"
#include "Rendering\Camera.h"

namespace CADMageddon
{
    class CursorController
    {
    public:
        CursorController() = default;

        void Resize(const glm::vec2& viewPortSize, FPSCamera& camera);

        void Update(Timestep ts, FPSCamera& camera, glm::vec2 mousePosition);

        const Cursor3D getCursor() const { return m_Cursor; }

    private:
        glm::vec2 GetCursorScreenPosition(FPSCamera& camera);
        glm::vec3 GetCursorWorldPosition(FPSCamera& camera);

    private:
        glm::vec2 m_ViewPortSize = { 0.0f,0.0f };
        Cursor3D m_Cursor;
    };
}