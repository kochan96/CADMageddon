#pragma once
#include "cadpch.h"
#include "Point.h"
#include "Curve.h"

namespace CADMageddon
{
    class InterpolatedCurve : public Curve
    {
    public:
        InterpolatedCurve(std::string name) :Curve(name) {}
        std::vector<glm::vec3> GetBezierControlPoints() const;

        bool GetShowPolygon() const { return m_ShowPolygon; }
        void SetShowPolygon(bool showPolygon) { m_ShowPolygon = showPolygon; }

        bool GetShowPoints() const { return m_ShowPoints; }
        void SetShowPoints(bool setShowPoints);

    private:
        bool m_ShowPoints = true;
        bool m_ShowPolygon = false;
    };
}