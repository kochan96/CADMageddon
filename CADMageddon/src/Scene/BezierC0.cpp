#include "BezierC0.h"

namespace CADMageddon
{
    void BezierC0::SetShowPoints(bool setShowPoints)
    {
        m_ShowPoints = setShowPoints;
        for (auto point : m_ControlPoints)
        {
            point->SetIsVisible(setShowPoints);
        }
    }
}