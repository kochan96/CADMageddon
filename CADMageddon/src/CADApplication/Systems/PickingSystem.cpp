#include "PickingSystem.h"
#include "Core\Input.h"
#include "TransformationSystem.h"
#include "Scene\Scene.h"

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

        auto points = m_Scene.GetPoints();

        for (auto point : points)
        {
            glm::vec4 worldPosition = point->GetTransform()->GetMatrix() * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
            auto frustumPosition = camera.GetViewProjectionMatrix() * worldPosition;
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

            point->SetIsSelected(!point->GetIsSelected());
            if (point->GetIsSelected())
            {
                m_TransformationSystem->AddToSelected(point->GetTransform());
            }
            else
            {
                m_TransformationSystem->RemoveFromSelected(point->GetTransform());
            }
        }

        for (auto torus : m_Scene.GetTorus())
        {
            glm::vec4 worldPosition = torus->GetTransform()->GetMatrix() * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
            auto frustumPosition = camera.GetViewProjectionMatrix() * worldPosition;
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

            torus->SetIsSelected(!torus->GetIsSelected());
            if (torus->GetIsSelected())
            {
                m_TransformationSystem->AddToSelected(torus->GetTransform());
            }
            else
            {
                m_TransformationSystem->RemoveFromSelected(torus->GetTransform());
            }
        }
    }

    void PickingSystem::ClearSelection(const Scene& scene)
    {
        for (auto point : scene.GetPoints())
        {
            point->SetIsSelected(false);
        }

        for (auto torus : scene.GetTorus())
        {
            torus->SetIsSelected(false);
        }

        m_TransformationSystem->ClearSelection();
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
