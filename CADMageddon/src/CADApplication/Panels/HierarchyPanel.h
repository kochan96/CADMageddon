#pragma once
#include "cadpch.h"
#include "Core/Base.h"

namespace CADMageddon
{
    class Scene;
    class Point;
    class Torus;
    class BezierC0;

    class HierarchyPanel
    {
    public:
        HierarchyPanel(Ref<Scene> scene);
        void Render();

        void SetOnSelectionChangedPointCallback(std::function<void(bool, Ref<Point>)> selectionPointChangedCallback)
        {
            m_OnSelectionPointChanged = selectionPointChangedCallback;
        }

        void SetOnSelectionChangedTorusCallback(std::function<void(bool, Ref<Torus>)> selectionTorusChangedCallback)
        {
            m_OnSelectionTorusChanged = selectionTorusChangedCallback;
        }

        void SetOnSelectionClearedCallback(std::function<void()> selectionClearedCallback)
        {
            m_OnSelectionCleared = selectionClearedCallback;
        }

        void SetOnBezierSelectionChangedCallback(std::function<void(bool, Ref<BezierC0>)> selectionBezierChangedCallback)
        {
            m_OnSelectionBezierC0Changed = selectionBezierChangedCallback;
        }

        void SetScene(Ref<Scene> scene) { m_Scene = scene; }

    private:
        void ClearSelection();
        void RenderPointNode(Ref<Point> point, int& id);
        void RenderTorusNode(Ref<Torus> torus, int& id);
        void RenderBezierC0Node(Ref<BezierC0> bezierC0, int& id);
        void RenderBezierControlPointNode(Ref<BezierC0> bezierc0, Ref<Point> point, int& id);

    private:
        std::function<void(bool, Ref<Point>)> m_OnSelectionPointChanged;
        std::function<void(bool, Ref<Torus>)> m_OnSelectionTorusChanged;
        std::function<void(bool, Ref<BezierC0>)> m_OnSelectionBezierC0Changed;
        std::function<void()> m_OnSelectionCleared;
        Ref<Scene> m_Scene;

    };
}