#pragma once
#include "cadpch.h"
#include "Point.h"
#include "Curve.h"

namespace CADMageddon
{
    class BSpline : public Curve
    {
    public:
        BSpline(std::string name) :m_Name(name) {}

        std::vector<glm::vec3> GetBezierControlPoints() const;

        bool GetShowBSplinePolygon() const { return m_ShowBSplinePolygon; }
        void SetShowBSplinePolygon(bool showPolygon) { m_ShowBSplinePolygon = showPolygon; }

        bool GetShowBezierPolygon() const { return m_ShowBezierPolygon; }
        void SetShowBezierPolygon(bool showPolygon) { m_ShowBezierPolygon = showPolygon; }

        bool GetIsSelected() const { return m_IsSelected; }
        void SetIsSelected(bool isSelected) { m_IsSelected = isSelected; }

        std::string GetName() const { return m_Name; }
        void SetName(std::string name) { m_Name = name; }

        bool GetIsBezierBasis() const { return m_IsBezierBasis; }
        void SetIsBezierBasis(bool isBezierBasis) { m_IsBezierBasis = isBezierBasis; }

        bool GetShowPoints() const { return m_ShowPoints; }
        void SetShowPoints(bool setShowPoints);

        bool GetSnapToEnd() const { return m_SnapToEnd; }
        void SetSnapToEnd(bool snap) { m_SnapToEnd = snap; }

    private:
        std::string m_Name;
        bool m_ShowPoints = true;
        bool m_IsSelected = false;
        bool m_ShowBSplinePolygon = false;
        bool m_ShowBezierPolygon = false;
        bool m_IsBezierBasis = false;

        bool m_SnapToEnd = false;
    };
}