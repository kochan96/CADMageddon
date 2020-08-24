#include "BSpline.h"

namespace CADMageddon
{
    std::vector<glm::vec3> BSpline::GetBezierControlPoints() const
    {
        std::vector<glm::vec3> deBoors;
        std::transform(m_ControlPoints.begin(), m_ControlPoints.end(), std::back_inserter(deBoors), [](Ref<Point> x) {return x->GetPosition(); });

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


    void BSpline::AddControlPoint(Ref<Point> point)
    {
        m_ControlPoints.push_back(point);
    }

    void BSpline::RemoveControlPoint(Ref<Point> point)
    {
        auto it = std::find(m_ControlPoints.begin(), m_ControlPoints.end(), point);
        if (it != m_ControlPoints.end())
            m_ControlPoints.erase(it);
    }

}