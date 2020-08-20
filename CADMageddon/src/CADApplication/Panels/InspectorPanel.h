#pragma once
#include "cadpch.h"
#include "Scene\Point.h"
#include "Scene\Torus.h"

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
        void Clear();

    private:
        std::vector<Ref<Point>> m_Points;
        std::vector<Ref<Torus>> m_Torus;
    };
}