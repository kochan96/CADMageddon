#pragma once
#include "cadpch.h"
#include "Core\Base.h"
#include "Rendering\Camera.h"


namespace CADMageddon
{
    class TransformationSystem;
    class Scene;

    class PickingSystem
    {
    public:
        PickingSystem(Ref<TransformationSystem> transformationSystem);

        void Update(
            const glm::vec2& mousePosition,
            const glm::vec2& viewPortSize,
            const Scene& m_Scene,
            const FPSCamera& camera);

    private:
        Ref<TransformationSystem> m_TransformationSystem;
        void ClearSelection(const Scene& scene);
        bool IsInsideFrustum(const glm::vec4& position);
        bool IsInsidePickingArea(const glm::vec2& position, const glm::vec2& mousePosition, const float pickingDistance);
    };
}