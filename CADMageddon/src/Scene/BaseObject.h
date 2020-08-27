#pragma once
#include "Point.h"
#include "cadpch.h"

namespace CADMageddon
{
    class BaseObject
    {
    public:
        std::vector<Ref<Point>>& GetControlPoints() { return m_ControlPoints; }

    protected:
        std::vector<Ref<Point>> m_ControlPoints;
    };
}