#pragma once
#include "cadpch.h"
#include "Point.h"
#include "Core/Base.h"

namespace CADMageddon
{
    class BezierC0
    {
    public:
        BezierC0(std::string name) :m_Name(name) {}

        std::vector<Ref<Point>> GetControlPoints() const { return m_ControlPoints; }
        void AddControlPoint(Ref<Point> point);
        void RemoveControlPoint(Ref<Point> point);

        bool GetShowPolygon() const { return m_ShowPolygon; }
        void SetShowPolygon(bool showPolygon) { m_ShowPolygon = showPolygon; }

        bool GetIsSelected() const { return m_IsSelected; }
        void SetIsSelected(bool isSelected) { m_IsSelected = isSelected; }

        std::string GetName() const { return m_Name; }
        void SetName(std::string name) { m_Name = name; }

        bool GetShowPoints() const { return m_ShowPoints; }
        void SetShowPoints(bool setShowPoints);

    private:
        std::string m_Name;
        bool m_IsSelected = false;
        bool m_ShowPoints = true;
        bool m_ShowPolygon = false;
        std::vector<Ref<Point>> m_ControlPoints;

    };
}