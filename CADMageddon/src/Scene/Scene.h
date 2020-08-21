#pragma once
#include "Point.h"
#include "Torus.h"
#include "BezierC0.h"
#include "Core\Base.h"

namespace CADMageddon
{
    class Scene
    {
    public:
        Scene();
        void Update();
        void DeleteSelected();
        void AssignSelectedFreeToBezier(Ref<BezierC0> bezier);
        void RemovePointFromBezier(Ref<BezierC0> bezier, Ref<Point> point);


        Ref<Point> CreatePoint(glm::vec3 position, std::string name);
        Ref<Torus> CreateTorus(glm::vec3 position, std::string name);
        Ref<BezierC0> CreateBezierC0(std::string name);

        std::vector<Ref<Point>> GetFreePoints() const { return m_FreePoints; }
        std::vector<Ref<Point>> GetPoints() const { return m_Points; }
        std::vector<Ref<Torus>> GetTorus() const { return m_Torus; }
        std::vector<Ref<BezierC0>> GetBezierC0() const { return m_BezierC0; }

    private:
        void DeleteFreePoint(Ref<Point> point);
        void DeleteTorus(Ref<Torus> torus);
        void DeleteBezierC0(Ref<BezierC0> bezierC0);

    private:
        std::vector<Ref<Point>> m_FreePoints;
        std::vector<Ref<Point>> m_Points;
        std::vector<Ref<Torus>> m_Torus;
        std::vector<Ref<BezierC0>> m_BezierC0;
    };
}