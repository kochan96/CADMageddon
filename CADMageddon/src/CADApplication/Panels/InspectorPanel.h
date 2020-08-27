#pragma once
#include "cadpch.h"
#include "Scene\Scene.h"
#include "CADApplication\Systems\TransformationSystem.h"

namespace CADMageddon
{
    class InspectorPanel
    {
    public:
        InspectorPanel(Ref<Scene> scene, Ref<TransformationSystem> transformationSystem)
            :m_transformationSystem(transformationSystem), m_Scene(scene) {};

        void SetScene(Ref<Scene> scene) { m_Scene = scene; }

        void Render();

        void AddPoint(Ref<Point> point);
        void AddTorus(Ref<Torus> point);

        void RemovePoint(Ref<Point> entity);
        void RemoveTorus(Ref<Torus> torus);

        void AddBezierC0(Ref<BezierC0> bezier);
        void RemoveBezierC0(Ref<BezierC0> bezier);

        void AddBSpline(Ref<BSpline> bSpline);
        void RemoveBSpline(Ref<BSpline> bSpline);

        void AddInterpolatedCurve(Ref<InterpolatedCurve> interPolatedCurve);
        void RemoveInterpolatedCurve(Ref<InterpolatedCurve> interPolatedCurve);

        void AddBezierPatch(Ref<BezierPatch> bezierPatch);
        void RemoveBezierPatch(Ref<BezierPatch> bezierPatch);

        void AddBSplinePatch(Ref<BSplinePatch> bSplinePatch);
        void RemoveBSplinePatch(Ref<BSplinePatch> bSplinePatch);

        void ClearPointsAndToruses();
        void Clear();

    private:
        void RenderMultiSelectInspector();
        bool CheckIfFillHolePossible(Ref<BezierPatch> b1, Ref<BezierPatch> b2, Ref<BezierPatch> b3);


    private:
        Ref<Scene> m_Scene;
        Ref<TransformationSystem> m_transformationSystem;
        std::vector<Ref<Point>> m_Points;
        std::vector<Ref<Torus>> m_Torus;
        std::vector<Ref<BezierC0>> m_Bezier;
        std::vector<Ref<BSpline>> m_BSpline;
        std::vector<Ref<InterpolatedCurve>> m_InterPolatedCurve;
        std::vector<Ref<BezierPatch>> m_BezierPatch;
        std::vector<Ref<BSplinePatch>> m_BSplinePatch;
    };
}