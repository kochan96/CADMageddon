#include "InterpolatedCurve.h"

namespace CADMageddon
{
    std::vector<glm::vec3> CADMageddon::InterpolatedCurve::GetBezierControlPoints() const
    {
        if (m_ControlPoints.empty())
            return std::vector<glm::vec3 >();

        std::vector<glm::vec3> knots;
        for (int i = 0; i < m_ControlPoints.size(); i++) {
            glm::vec3 knot = m_ControlPoints[i]->GetPosition();
            if (i == 0 || knot != knots[knots.size() - 1])
                knots.emplace_back(knot);
        }
        int knotCount = knots.size();
        std::vector<glm::vec3> a(knotCount - 1), b(knotCount - 1), c(knotCount), d(knotCount - 1);
        std::vector<float> alfa(knotCount - 1), beta(knotCount - 1), diag(knotCount - 1),
            dist(knotCount - 1);
        std::vector<glm::vec3> bezierPoints(3 * knotCount - 2);
        for (int i = 0; i < knotCount - 1; i++)
            dist[i] = glm::distance(knots[i], knots[i + 1]);
        for (int i = 1; i < knotCount - 1; i++) {
            alfa[i] = dist[i - 1] / (dist[i - 1] + dist[i]);
            beta[i] = dist[i] / (dist[i - 1] + dist[i]);
            diag[i] = 2;
            c[i] = 3.f * ((knots[i + 1] - knots[i]) / dist[i] -
                (knots[i] - knots[i - 1]) / dist[i - 1]) / (dist[i - 1] + dist[i]);
        }

        //solving equation
        if (knotCount > 2) {
            beta[1] /= diag[1];
            c[1] /= diag[1];
            for (int i = 2; i < knotCount - 1; i++) {
                float m = 1.0f / (diag[i] - alfa[i] * beta[i - 1]);
                beta[i] *= m;
                c[i] = (c[i] - alfa[i] * c[i - 1]) * m;
            }
            for (int i = knotCount - 3; i > 1; i--)
                c[i] -= beta[i] * c[i + 1];
            c[1] -= beta[1] * c[2];
        }
        c[0] = c[knotCount - 1] = glm::vec3(0);

        for (int i = 0; i < knotCount - 1; i++) {
            a[i] = knots[i];
            d[i] = (c[i + 1] - c[i]) / (3.f * dist[i]);
            c[i] *= dist[i] * dist[i];
            d[i] *= dist[i] * dist[i] * dist[i];
            b[i] = knots[i + 1] - a[i] - c[i] - d[i];

            bezierPoints[3 * i] = knots[i];
            bezierPoints[3 * i + 1] = a[i] + 1.f / 3.f * b[i];
            bezierPoints[3 * i + 2] = a[i] + 2.f / 3.f * b[i] + 1.f / 3.f * c[i];
        }

        bezierPoints[3 * knotCount - 3] = knots[knotCount - 1];
        return bezierPoints;
    }


    void InterpolatedCurve::AddControlPoint(Ref<Point> point)
    {
        m_ControlPoints.push_back(point);
    }

    void InterpolatedCurve::RemoveControlPoint(Ref<Point> point)
    {
        auto it = std::find(m_ControlPoints.begin(), m_ControlPoints.end(), point);
        if (it != m_ControlPoints.end())
            m_ControlPoints.erase(it);
    }
}

