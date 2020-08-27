#include "PickingSystem.h"
#include "Core\Input.h"
#include "TransformationSystem.h"
#include "Scene\Scene.h"
#include "Rendering\Renderer.h"

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
            m_IsMultiSelect = false;
        }
        else
        {
            m_IsMultiSelect = true;
            multiSelectStart = mousePosition;
            multiSelectEnd = mousePosition;
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
            if (m_OnPointSelectionChanged)
                m_OnPointSelectionChanged(point->GetIsSelected(), point);
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
            if (m_OnTorusSelectionChanged)
                m_OnTorusSelectionChanged(torus->GetIsSelected(), torus);
        }
    }

    void PickingSystem::UpdateMultiSelect(const glm::vec2& mousePosition, const glm::vec2& viewPortSize, const Scene& m_Scene, const FPSCamera& camera)
    {
        if (!m_IsMultiSelect)
        {
            return;
        }

        if (Input::IsKeyPressed(CDM_KEY_LEFT_CONTROL) == false || Input::IsMouseButtonPressed(MouseCode::ButtonLeft) == false)
        {
            m_IsMultiSelect = false;
        }

        if (mousePosition == multiSelectEnd)
        {
            RenderPickingBox(viewPortSize);
            return;
        }


        ClearSelection(m_Scene);
        multiSelectEnd = mousePosition;
        RenderPickingBox(viewPortSize);

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
            if (!IsInsidePickingBox(multiSelectStart, multiSelectEnd, screenPosition))
            {
                continue;
            }

            point->SetIsSelected(true);
            if (m_OnPointSelectionChanged)
                m_OnPointSelectionChanged(true, point);
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
            if (!IsInsidePickingBox(multiSelectStart, multiSelectEnd, screenPosition))
            {
                continue;
            }

            torus->SetIsSelected(true);
            if (m_OnTorusSelectionChanged)
                m_OnTorusSelectionChanged(true, torus);
        }
    }

    void PickingSystem::RenderPickingBox(const glm::vec2& viewPortSize)
    {
        glm::vec4 COLOR_SELECTION_BOX_BORDER(0.2, 0.65, 1.0, 1.0);
        glm::vec4 COLOR_SELECTION_BOX_FILL(0.2, 0.65, 1.0, 0.1);

        auto ndcStart = toNDC(multiSelectStart, viewPortSize);
        auto ndcEnd = toNDC(multiSelectEnd, viewPortSize);

        Renderer::RenderScreenQuad(ndcStart, ndcEnd, COLOR_SELECTION_BOX_FILL);
        Renderer::RenderScreenQuadBorder(ndcStart, ndcEnd, COLOR_SELECTION_BOX_BORDER);
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

        m_OnSelectionCleared();
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

    bool PickingSystem::IsInsidePickingBox(const glm::vec2& selectionBoxStart, const glm::vec2& selectionBoxEnd, const  glm::vec2& position)
    {
        float minX = glm::min(selectionBoxEnd.x, selectionBoxStart.x);
        float minY = glm::min(selectionBoxEnd.y, selectionBoxStart.y);
        float maxX = glm::max(selectionBoxEnd.x, selectionBoxStart.x);
        float maxY = glm::max(selectionBoxEnd.y, selectionBoxStart.y);

        if (position.x >= minX && position.x <= maxX && position.y > minY && position.y < maxY)
            return true;

        return false;
    }

    glm::vec2 PickingSystem::toNDC(const glm::vec2& position, const glm::vec2& viewPortSize)
    {
        glm::vec2 ndc;

        ndc.x = (position.x * 2.0f) / viewPortSize.x - 1.0f;
        ndc.y = 1.0f - (position.y * 2.0f) / viewPortSize.y;

        return ndc;
    }

}
