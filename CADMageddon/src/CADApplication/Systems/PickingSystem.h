#pragma once
#include "cadpch.h"
#include "Rendering\Camera.h"
#include "Scene\Entity.h"
#include "Scene\Components.h"

namespace CADMageddon
{
    class PickingSystem
    {
    public:
        void Update(
            const glm::vec2& mousePosition,
            const glm::vec2& viewPortSize,
            const Scene& m_Scene,
            const FPSCamera& camera);

    private:
        void ClearSelection(Scene& scene);
        bool IsInsideFrustum(const glm::vec3& position);
        bool IsInsidePickingArea(const glm::vec2& position, const glm::vec2& mousePosition, const float pickingDistance);
    };
}