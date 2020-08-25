#pragma once
#include "cadpch.h"
#include "Core/Base.h"

namespace CADMageddon
{
    class Scene;
    class Point;
    class Torus;
    class BezierC0;
    class BSpline;
    class InterpolatedCurve;
    class BezierPatch;

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

        void SetOnBSplineSelectionChangedCallback(std::function<void(bool, Ref<BSpline>)> selectionBSplineChangedCallback)
        {
            m_OnSelectionBSplineChanged = selectionBSplineChangedCallback;
        }

        void SetOnInterpolatedSelectionChangedCallback(std::function<void(bool, Ref<InterpolatedCurve>)> selectionInterpolatedChangedCallback)
        {
            m_OnSelectionInterpolatedChanged = selectionInterpolatedChangedCallback;
        }

        void SetOnBezierPatchSelectionChanged(std::function<void(bool, Ref<BezierPatch>)> selectionBezierPatchChangedCallback)
        {
            m_OnBezierPatchSelectionChanged = selectionBezierPatchChangedCallback;
        }

        void SetScene(Ref<Scene> scene) { m_Scene = scene; }

    private:
        void ClearSelection();
        void RenderPointNode(Ref<Point> point, int& id);
        void RenderTorusNode(Ref<Torus> torus, int& id);

        void RenderBezierC0Node(Ref<BezierC0> bezierC0, int& id);
        void RenderBezierControlPointNode(Ref<BezierC0> bezierc0, Ref<Point> point, int& id);

        void RenderBSplineNode(Ref<BSpline> bSpline, int& id);
        void RenderBSplineControlPointNode(Ref<BSpline> bSpline, Ref<Point> point, int& id);

        void RenderInterpolatedCurveNode(Ref<InterpolatedCurve> interpolatedCurve, int& id);
        void RenderIntrepolaterdCurveControlPointNode(Ref<InterpolatedCurve> interpolatedCurve, Ref<Point> point, int& id);

        void RenderBezierPatchRectNode(Ref<BezierPatch> bezierPatch, int& id);
        void RenderBezierPatchControlPointNode(Ref<BezierPatch> bezierPatch, Ref<Point> point, int& id);

    private:
        std::function<void(bool, Ref<Point>)> m_OnSelectionPointChanged;
        std::function<void(bool, Ref<Torus>)> m_OnSelectionTorusChanged;
        std::function<void(bool, Ref<BezierC0>)> m_OnSelectionBezierC0Changed;
        std::function<void(bool, Ref<BSpline>)> m_OnSelectionBSplineChanged;
        std::function<void(bool, Ref<InterpolatedCurve>)> m_OnSelectionInterpolatedChanged;
        std::function<void(bool, Ref<BezierPatch>)> m_OnBezierPatchSelectionChanged;
        std::function<void()> m_OnSelectionCleared;
        Ref<Scene> m_Scene;

    };
}