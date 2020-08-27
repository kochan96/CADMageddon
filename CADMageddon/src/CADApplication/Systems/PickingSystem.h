#pragma once
#pragma once
#include "cadpch.h"
#include "Core\Base.h"
#include "Rendering\Camera.h"


namespace CADMageddon
{
    class TransformationSystem;
    class Scene;
    class Point;
    class Torus;

    //TODO selection box

    class PickingSystem
    {
    public:
        PickingSystem(Ref<TransformationSystem> transformationSystem);

        void Update(
            const glm::vec2& mousePosition,
            const glm::vec2& viewPortSize,
            const Scene& m_Scene,
            const FPSCamera& camera);

        void UpdateMultiSelect(
            const glm::vec2& mousePosition,
            const glm::vec2& viewPortSize,
            const Scene& m_Scene,
            const FPSCamera& camera);


        void SetOnPointSelectionChanged(std::function<void(bool, Ref<Point>)> pointSelectionCallback)
        {
            m_OnPointSelectionChanged = pointSelectionCallback;
        }

        void SetOnTorusSelectionChanged(std::function<void(bool, Ref<Torus>)> torusSelectionCallback)
        {
            m_OnTorusSelectionChanged = torusSelectionCallback;
        }

        void SetOnSelectionCleared(std::function<void()> selectionClearedCallback)
        {
            m_OnSelectionCleared = selectionClearedCallback;
        }

    private:
        void RenderPickingBox(const glm::vec2& viewPortSize);

    private:
        std::function<void(bool, Ref<Point>)> m_OnPointSelectionChanged;
        std::function<void(bool, Ref<Torus>)> m_OnTorusSelectionChanged;
        std::function<void()> m_OnSelectionCleared;


        glm::vec2 multiSelectStart;
        glm::vec2 multiSelectEnd;
        bool m_IsMultiSelect = false;
        Ref<TransformationSystem> m_TransformationSystem;
        void ClearSelection(const Scene& scene);
        bool IsInsideFrustum(const glm::vec4& position);
        bool IsInsidePickingArea(const glm::vec2& position, const glm::vec2& mousePosition, const float pickingDistance);
        bool IsInsidePickingBox(const glm::vec2& selectionBoxStart, const glm::vec2& selectionBoxEnd, const glm::vec2& position);

        glm::vec2 toNDC(const glm::vec2& position,const glm::vec2 &viewPortSize);
    };
}