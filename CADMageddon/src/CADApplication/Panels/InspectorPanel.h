#pragma once
#include "cadpch.h"
#include "Scene\Scene.h"
#include "CADApplication\Systems\TransformationSystem.h"
#include "CADApplication\Systems\Cursor3D.h"

namespace CADMageddon
{
    class InspectorPanel
    {
    public:
        InspectorPanel(Ref<Scene> scene, Ref<TransformationSystem> transformationSystem, Ref<Cursor3D> cursor)
            :m_transformationSystem(transformationSystem), m_Scene(scene), m_Cursor(cursor) {};

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

        void AddGregoryPatch(Ref<GregoryPatch> gregoryPatch);
        void RemoveGregoryPatch(Ref<GregoryPatch> gregoryPatch);

        void AddIntersectionCurve(Ref<IntersectionCurve> intersectionCurve);
        void RemoveIntersectionCurve(Ref<IntersectionCurve> intersectionCurve);

        void ClearPointsAndToruses();
        void Clear();

    private:
        void RenderIntersectionPlot(Ref<IntersectionCurve> intersectionCurve);

        void RenderMultiSelectInspector();
        void RenderFillHoleInspector();
        void RenderFindIntersectionInspector();
        void GetIntersectionSurfaces(Ref<SurfaceUV>& s1, Ref<SurfaceUV>& s2);

        bool GetCommonPoint(Ref<BezierPatch> b1, Ref<BezierPatch> b2, Ref<Point>& commonPoint);
        bool CheckIfCorner(Ref<BezierPatch> b, Ref<Point> commonPoint);


    private:
        bool m_IntersectionNotFound = false;
        bool m_BeginFromCursor = false;
        float m_StepLength = 0.001f;

        Ref<Scene> m_Scene;
        Ref<TransformationSystem> m_transformationSystem;
        Ref<Cursor3D> m_Cursor;
        std::vector<Ref<Point>> m_Points;
        std::vector<Ref<Torus>> m_Torus;
        std::vector<Ref<BezierC0>> m_Bezier;
        std::vector<Ref<BSpline>> m_BSpline;
        std::vector<Ref<InterpolatedCurve>> m_InterPolatedCurve;
        std::vector<Ref<BezierPatch>> m_BezierPatch;
        std::vector<Ref<BSplinePatch>> m_BSplinePatch;
        std::vector<Ref<GregoryPatch>> m_GregoryPatch;
        std::vector<Ref<SurfaceUV>> m_Surfaces;
        std::vector<Ref<IntersectionCurve>> m_IntersectionCurves;
    };
}