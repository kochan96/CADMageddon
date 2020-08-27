#include "BSpline.h"

namespace CADMageddon
{
    void BSpline::SetShowPoints(bool setShowPoints)
    {
        m_ShowPoints = setShowPoints;
        for (auto point : m_ControlPoints)
        {
            point->SetIsVisible(setShowPoints);
        }
    }

    std::vector<glm::vec3> BSpline::GetBezierControlPoints() const
    {
     
        if (m_SnapToEnd && m_ControlPoints.empty())
        {
            return std::vector<glm::vec3>();
        }
        else if (!m_SnapToEnd && m_ControlPoints.size() < 4)
        {
            return std::vector<glm::vec3>();
        }

        std::vector<glm::vec3> deBoors;
        if (m_SnapToEnd)
        {
            deBoors.push_back(m_ControlPoints.front()->GetPosition());
            deBoors.push_back(m_ControlPoints.front()->GetPosition());
        }

        std::transform(m_ControlPoints.begin(), m_ControlPoints.end(), std::back_inserter(deBoors), [](Ref<Point> x) {return x->GetPosition(); });

        if (m_SnapToEnd)
        {
            deBoors.push_back(m_ControlPoints.back()->GetPosition());
            deBoors.push_back(m_ControlPoints.back()->GetPosition());
        }

        std::vector<glm::vec3> bezierCoords;
        std::vector<glm::vec3> midPoints;

        for (int i = 1; i < deBoors.size(); i++) {
            midPoints.emplace_back(deBoors[i - 1] * 2.f / 3.f + deBoors[i] * 1.f / 3.f);
            midPoints.emplace_back(deBoors[i - 1] * 1.f / 3.f + deBoors[i] * 2.f / 3.f);
        }
        for (int i = 1; i < midPoints.size() - 2; i++) {
            if (i > 1)
                bezierCoords.emplace_back(midPoints[i]);
            if (i % 2 == 1)
                bezierCoords.emplace_back((midPoints[i] + midPoints[i + 1]) / 2.f);
        }

        return bezierCoords;
    }
}