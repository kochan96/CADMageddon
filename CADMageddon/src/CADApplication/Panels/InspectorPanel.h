#pragma once
#include "cadpch.h"
#include "Scene\Point.h"
#include "Scene\Torus.h"
#include "Scene\BezierC0.h"
#include "Scene\BSpline.h"
#include "Scene\InterpolatedCurve.h"

namespace CADMageddon
{
    class InspectorPanel
    {
    public:
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

        void Clear();

    private:
        std::vector<Ref<Point>> m_Points;
        std::vector<Ref<Torus>> m_Torus;
        std::vector<Ref<BezierC0>> m_Bezier;
        std::vector<Ref<BSpline>> m_BSpline;
        std::vector<Ref<InterpolatedCurve>> m_InterPolatedCurve;
    };
}