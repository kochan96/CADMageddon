#include "BezierC0.h"

namespace CADMageddon
{
    void BezierC0::AddControlPoint(Ref<Point> point)
    {
        m_ControlPoints.push_back(point);
    }

    void BezierC0::RemoveControlPoint(Ref<Point> point)
    {
        auto it = std::find(m_ControlPoints.begin(), m_ControlPoints.end(), point);
        if (it != m_ControlPoints.end())
            m_ControlPoints.erase(it);
    }

}