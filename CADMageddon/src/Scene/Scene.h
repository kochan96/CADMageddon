#pragma once
#include "Point.h"
#include "Torus.h"
#include "BezierC0.h"
#include "BSpline.h"
#include "InterpolatedCurve.h"

namespace CADMageddon
{
    class Scene
    {
    public:
        Scene();
        void Update();
        void DeleteSelected();
        void AssignSelectedFreeToBezier(Ref<BezierC0> bezier);
        void AssignSelectedFreeToBSpline(Ref<BSpline> bSpline);
        void AssignSelectedFreeToInterpolated(Ref<InterpolatedCurve> interpolatedCurve);

        void RemovePointFromBezier(Ref<BezierC0> bezier, Ref<Point> point);
        void RemovePointFromBSpline(Ref<BSpline> bSpline, Ref<Point> point);
        void RemovePointFromInterpolated(Ref<InterpolatedCurve> interpolatedCurve, Ref<Point> point);


        Ref<Point> CreatePoint(glm::vec3 position, std::string name);
        Ref<Torus> CreateTorus(glm::vec3 position, std::string name);
        Ref<BezierC0> CreateBezierC0(std::string name);
        Ref<BSpline> CreateBSpline(std::string name);
        Ref<InterpolatedCurve> CreateInterpolated(std::string name);

        std::vector<Ref<Point>> GetFreePoints() const { return m_FreePoints; }
        std::vector<Ref<Point>> GetPoints() const { return m_Points; }
        std::vector<Ref<Torus>> GetTorus() const { return m_Torus; }
        std::vector<Ref<BezierC0>> GetBezierC0() const { return m_BezierC0; }
        std::vector<Ref<BSpline>> GetBSpline() const { return m_BSpline; }
        std::vector<Ref<InterpolatedCurve>> GetInterpolated() const { return m_InterpolatedCurve; }

    private:
        void RenderBezier(Ref<BezierC0> bezierC0);
        void RenderBSpline(Ref<BSpline> bSpline);
        void RenderInterpolatedCurve(Ref<InterpolatedCurve> interPolatedCurve);

        bool AddNewPointToBezier(Ref<Point> point);
        bool AddNewPointToBSpline(Ref<Point> point);
        bool AddNewPointToInterpolated(Ref<Point> point);

        void DeleteFreePoint(Ref<Point> point);
        void DeleteTorus(Ref<Torus> torus);
        void DeleteBezierC0(Ref<BezierC0> bezierC0);
        void DeleteBSpline(Ref<BSpline> bSpline);
        void DeleteInterpolatedCurve(Ref<InterpolatedCurve> interpolatedCurve);

    private:
        std::vector<Ref<Point>> m_FreePoints;
        std::vector<Ref<Point>> m_Points;
        std::vector<Ref<Torus>> m_Torus;
        std::vector<Ref<BezierC0>> m_BezierC0;
        std::vector<Ref<BSpline>> m_BSpline;
        std::vector < Ref<InterpolatedCurve>> m_InterpolatedCurve;
    };
}