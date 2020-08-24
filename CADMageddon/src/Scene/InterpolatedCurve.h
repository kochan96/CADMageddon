#pragma once
#include "cadpch.h"
#include "Point.h"

namespace CADMageddon
{
    class InterpolatedCurve
    {
    public:
        InterpolatedCurve(std::string name) :m_Name(name) {}

        std::vector<Ref<Point>> GetControlPoints() const { return m_ControlPoints; }
        std::vector<glm::vec3> GetBezierControlPoints() const;
        void AddControlPoint(Ref<Point> point);
        void RemoveControlPoint(Ref<Point> point);

        bool GetShowPolygon() const { return m_ShowPolygon; }
        void SetShowPolygon(bool showPolygon) { m_ShowPolygon = showPolygon; }

        bool GetIsSelected() const { return m_IsSelected; }
        void SetIsSelected(bool isSelected) { m_IsSelected = isSelected; }

        std::string GetName() const { return m_Name; }
        void SetName(std::string name) { m_Name = name; }

    private:
        std::string m_Name;
        bool m_IsSelected = false;
        bool m_ShowPolygon = false;
        std::vector<Ref<Point>> m_ControlPoints;

    };
}