#include "Curve.h"

namespace CADMageddon
{
    void Curve::AddControlPoint(Ref<Point> point)
    {
        int referenceCount = point->GetReferencedCount();
        point->SetReferencedCount(referenceCount + 1);
        m_ControlPoints.push_back(point);
    }

    void Curve::RemoveControlPoint(Ref<Point> controlPoint)
    {
        auto it = std::find(m_ControlPoints.begin(), m_ControlPoints.end(), controlPoint);
        while (it != m_ControlPoints.end())
        {
            m_ControlPoints.erase(it);
            it = std::find(m_ControlPoints.begin(), m_ControlPoints.end(), controlPoint);
        }

        int referenceCount = controlPoint->GetReferencedCount() - 1;
        controlPoint->SetReferencedCount(referenceCount);
    }
}
