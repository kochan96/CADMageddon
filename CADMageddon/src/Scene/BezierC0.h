#pragma once
#include "cadpch.h"
#include "Point.h"
#include "Core/Base.h"
#include "Curve.h"

namespace CADMageddon
{
    class BezierC0 : public Curve
    {
    public:
        BezierC0(std::string name) :Curve(name) {}

        bool GetShowPolygon() const { return m_ShowPolygon; }
        void SetShowPolygon(bool showPolygon) { m_ShowPolygon = showPolygon; }

        bool GetShowPoints() const { return m_ShowPoints; }
        void SetShowPoints(bool setShowPoints);


    private:
        bool m_ShowPoints = true;
        bool m_ShowPolygon = false;
    };
}