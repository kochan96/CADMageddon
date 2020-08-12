#include "PickingSystem.h"
#include "Core\Input.h"
#include "TransformationSystem.h"

namespace CADMageddon
{
    PickingSystem::PickingSystem(Ref<TransformationSystem> transformationSystem)
        :m_TransformationSystem(transformationSystem)
    {

    }

    void PickingSystem::Update(
        const glm::vec2& mousePosition,
        const glm::vec2& viewPortSize,
        const Scene& m_Scene,
        const FPSCamera& camera)
    {

        bool multiSelect = Input::IsKeyPressed(CDM_KEY_LEFT_CONTROL);

        if (!multiSelect)
        {
            ClearSelection(m_Scene);
        }

        const float pointSize = 5.0f;

        auto entities = m_Scene.GetEntities();

        for (auto entity : entities)
        {
            if (!entity.HasComponent<TransformComponent>())
            {
                continue;
            }

            if (!entity.HasComponent<PointComponent>())
            {
                continue;
            }

            auto& pointComponent = entity.GetComponent<PointComponent>();
            auto& transformComponent = entity.GetComponent<TransformComponent>();
            auto frustumPosition = camera.GetViewProjectionMatrix() * transformComponent.GetMatrix() * glm::vec4(pointComponent.Point.GetPosition(), 1.0f);
            if (!IsInsideFrustum(frustumPosition))
            {
                continue;
            }

            frustumPosition /= frustumPosition.w;

            float x = (frustumPosition.x + 1.0f) * viewPortSize.x / 2;
            float y = (1.0f - frustumPosition.y) * viewPortSize.y / 2;

            glm::vec2 screenPosition(x, y);
            if (!IsInsidePickingArea(screenPosition, mousePosition, pointSize))
            {
                continue;
            }

            if (entity.HasComponent<HierarchyComponent>())
            {
                auto& hierarchySelectable = entity.GetComponent<HierarchyComponent>();
                hierarchySelectable.IsSelected = !hierarchySelectable.IsSelected;

                if (hierarchySelectable.IsSelected)
                {
                    m_TransformationSystem->AddToSelected(entity);
                }
                else
                {
                    m_TransformationSystem->RemoveFromSelected(entity);
                }
            }


        }
    }

    void PickingSystem::ClearSelection(const Scene& scene)
    {
        for (auto entity : scene.GetEntities())
        {
            if (!entity.HasComponent<PointComponent>())
            {
                continue;
            }

            if (entity.HasComponent<HierarchyComponent>())
            {
                auto& hierarchySelectable = entity.GetComponent<HierarchyComponent>();
                hierarchySelectable.IsSelected = false;
            }
        }
    }

    bool PickingSystem::IsInsideFrustum(const glm::vec4& position)
    {
        if (position.x > position.w || position.x < -position.w)
        {
            return false;
        }

        if (position.y > position.w || position.y < -position.w)
        {
            return false;
        }

        if (position.z > position.w || position.y < -position.w)
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
