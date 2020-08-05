#include "PickingSystem.h"
namespace CADMageddon
{
    void PickingSystem::Update(
        const glm::vec2& mousePosition,
        const glm::vec2& viewPortSize,
        const Scene& m_Scene,
        const FPSCamera& camera)
    {
        const float pointSize = 5.0f;

        auto entities = m_Scene.GetEntities();

        for (auto entity : entities)
        {
            if (!entity.HasComponent<SceneSelectableComponent>())
            {
                continue;
            }

            if (!entity.HasComponent<PointComponent>())
            {
                continue;
            }

            auto& pointComponent = entity.GetComponent<PointComponent>();
            auto frustumPosition = camera.GetViewProjectionMatrix() * glm::vec4(pointComponent.Position, 1.0f);
            frustumPosition /= frustumPosition.w;
            if (!IsInsideFrustum(frustumPosition))
            {
                continue;
            }

            float x = (frustumPosition.x + 1.0f) * viewPortSize.x / 2;
            float y = (1.0f - frustumPosition.y) * viewPortSize.y / 2;

            glm::vec2 screenPosition(x, y);
            if (!IsInsidePickingArea(screenPosition, mousePosition, pointSize))
            {
                continue;
            }
          
            auto& sceneSelectable = entity.GetComponent<SceneSelectableComponent>();
            sceneSelectable.IsSelected = true;

            if (entity.HasComponent<HierarchyComponent>())
            {
                auto& hierarchySelectable = entity.GetComponent<HierarchyComponent>();
                hierarchySelectable.IsSelected = true;
            }
        }
    }

    void PickingSystem::ClearSelection(Scene& scene)
    {
        for (auto entity : scene.GetEntities())
        {
            if (!entity.HasComponent<SceneSelectableComponent>())
            {
                continue;
            }

            if (!entity.HasComponent<PointComponent>())
            {
                continue;
            }

            auto& sceneSelectable = entity.GetComponent<SceneSelectableComponent>();
            sceneSelectable.IsSelected = false;

            if (entity.HasComponent<HierarchyComponent>())
            {
                auto& hierarchySelectable = entity.GetComponent<HierarchyComponent>();
                hierarchySelectable.IsSelected = false;
            }
        }
    }

    bool PickingSystem::IsInsideFrustum(const glm::vec3& position)
    {
        if (position.x > 1.0f || position.x < -1.0f)
        {
            return false;
        }

        if (position.y > 1.0f || position.y < -1.0f)
        {
            return false;
        }

        if (position.z > 1.0f || position.y < -1.0f)
        {
            return false;
        }

        return true;
    }

    bool PickingSystem::IsInsidePickingArea(const glm::vec2& position, const glm::vec2& mousePosition, const float pickingRadius)
    {
        if (position.x > mousePosition.x + pickingRadius)
        {
            return false;
        }

        if (position.x < mousePosition.x - pickingRadius)
        {
            return false;
        }

        if (position.y > mousePosition.y + pickingRadius)
        {
            return false;
        }

        if (position.y < mousePosition.y - pickingRadius)
        {
            return false;
        }

        return true;
    }
}
