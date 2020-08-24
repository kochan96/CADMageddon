#include "Scene.h"
#include "Rendering\Renderer.h"

namespace CADMageddon
{
    Scene::Scene()
    {

    }

    Ref<Point> Scene::CreatePoint(glm::vec3 position, std::string name)
    {
        auto point = CreateRef<Point>(position, name);
        m_Points.push_back(point);
        auto added = AddNewPointToBezier(point);
        added |= AddNewPointToBSpline(point);
        added |= AddNewPointToInterpolated(point);

        if (!added)
            m_FreePoints.push_back(point);

        return point;
    }

    Ref<Torus> Scene::CreateTorus(glm::vec3 position, std::string name)
    {
        auto torus = CreateRef<Torus>(position, name);
        m_Torus.push_back(torus);
        return torus;
    }

    Ref<BezierC0> Scene::CreateBezierC0(std::string name)
    {
        auto bezierC0 = CreateRef<BezierC0>(name);
        m_BezierC0.push_back(bezierC0);
        return bezierC0;
    }

    Ref<BSpline> Scene::CreateBSpline(std::string name)
    {
        auto bSpline = CreateRef<BSpline>(name);
        m_BSpline.push_back(bSpline);
        return bSpline;
    }

    Ref<InterpolatedCurve> Scene::CreateInterpolated(std::string name)
    {
        auto interpolated = CreateRef<InterpolatedCurve>(name);
        m_InterpolatedCurve.push_back(interpolated);
        return interpolated;
    }

    void Scene::Update()
    {
        for (auto torus : m_Torus)
        {
            std::vector<glm::vec3> vertices;
            for (auto point : torus->GetPoints())
            {
                vertices.push_back(point->GetTransform()->Translation);
            }

            auto color = torus->GetIsSelected() ? Renderer::SELECTED_COLOR : Renderer::DEFAULT_COLOR;
            Renderer::RenderTorus(vertices, torus->GetIndices(), torus->GetTransform()->GetMatrix(), color);
        }

        for (auto bezierC0 : m_BezierC0)
        {
            RenderBezier(bezierC0);
        }

        for (auto bSpline : m_BSpline)
        {
            RenderBSpline(bSpline);
        }

        for (auto interpolated : m_InterpolatedCurve)
        {
            RenderInterpolatedCurve(interpolated);
        }

        for (auto point : m_FreePoints)
        {
            auto color = point->GetIsSelected() ? Renderer::SELECTED_COLOR : Renderer::DEFAULT_COLOR;
            glm::vec3 position = point->GetTransform()->GetMatrix() * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
            Renderer::RenderPoint(position, color);
        }
    }

    void Scene::DeleteSelected()
    {
        std::vector<Ref<Point>> pointsToDelete;
        std::vector<Ref<Torus>> torusToDelete;
        std::vector<Ref<BezierC0>> bezierC0ToDelete;
        std::vector<Ref<BSpline>> bSplineToDelete;
        std::vector<Ref<InterpolatedCurve>> interpolatedToDelete;

        for (auto point : m_FreePoints)
        {
            if (point->GetIsSelected())
                pointsToDelete.push_back(point);
        }

        for (auto torus : m_Torus)
        {
            if (torus->GetIsSelected())
                torusToDelete.push_back(torus);
        }

        for (auto bezierC0 : m_BezierC0)
        {
            if (bezierC0->GetIsSelected())
                bezierC0ToDelete.push_back(bezierC0);
        }

        for (auto bSpline : m_BSpline)
        {
            if (bSpline->GetIsSelected())
                bSplineToDelete.push_back(bSpline);
        }

        for (auto interpolated : m_InterpolatedCurve)
        {
            if (interpolated->GetIsSelected())
                interpolatedToDelete.push_back(interpolated);
        }

        for (auto point : pointsToDelete)
        {
            DeleteFreePoint(point);
        }

        for (auto torus : torusToDelete)
        {
            DeleteTorus(torus);
        }

        for (auto bezierC0 : bezierC0ToDelete)
        {
            DeleteBezierC0(bezierC0);
        }

        for (auto bSpline : bSplineToDelete)
        {
            DeleteBSpline(bSpline);
        }

        for (auto interpolated : interpolatedToDelete)
        {
            DeleteInterpolatedCurve(interpolated);
        }
    }

    void Scene::AssignSelectedFreeToBezier(Ref<BezierC0> bezier)
    {
        auto points = m_FreePoints;
        for (auto point : points)
        {
            if (point->GetIsSelected())
            {
                bezier->AddControlPoint(point);
                auto it = std::find(m_FreePoints.begin(), m_FreePoints.end(), point);
                m_FreePoints.erase(it);
            }
        }
    }

    void Scene::AssignSelectedFreeToBSpline(Ref<BSpline> bSpline)
    {
        auto points = m_FreePoints;
        for (auto point : points)
        {
            if (point->GetIsSelected())
            {
                bSpline->AddControlPoint(point);
                auto it = std::find(m_FreePoints.begin(), m_FreePoints.end(), point);
                m_FreePoints.erase(it);
            }
        }
    }

    void Scene::AssignSelectedFreeToInterpolated(Ref<InterpolatedCurve> interpolatedCurve)
    {
        auto points = m_FreePoints;
        for (auto point : points)
        {
            if (point->GetIsSelected())
            {
                interpolatedCurve->AddControlPoint(point);
                auto it = std::find(m_FreePoints.begin(), m_FreePoints.end(), point);
                m_FreePoints.erase(it);
            }
        }
    }

    void Scene::RemovePointFromBezier(Ref<BezierC0> bezier, Ref<Point> point)
    {
        m_FreePoints.push_back(point);
        bezier->RemoveControlPoint(point);
    }

    void Scene::RemovePointFromBSpline(Ref<BSpline> bSpline, Ref<Point> point)
    {
        m_FreePoints.push_back(point);
        bSpline->RemoveControlPoint(point);
    }

    void Scene::RemovePointFromInterpolated(Ref<InterpolatedCurve> interpolatedCurve, Ref<Point> point)
    {
        m_FreePoints.push_back(point);
        interpolatedCurve->RemoveControlPoint(point);
    }

    bool Scene::AddNewPointToBezier(Ref<Point> point)
    {
        bool added = false;
        for (auto bezier : m_BezierC0)
        {
            if (bezier->GetIsSelected())
            {
                added = true;
                bezier->AddControlPoint(point);

                point = CreateRef<Point>(point->GetPosition(), point->GetName());
            }
        }


        return added;
    }

    bool Scene::AddNewPointToBSpline(Ref<Point> point)
    {
        bool added = false;
        for (auto bSpline : m_BSpline)
        {
            if (bSpline->GetIsSelected())
            {
                added = true;
                bSpline->AddControlPoint(point);
                point = CreateRef<Point>(point->GetPosition(), point->GetName());
            }
        }


        return added;
    }

    bool Scene::AddNewPointToInterpolated(Ref<Point> point)
    {
        bool added = false;
        for (auto interpolated : m_InterpolatedCurve)
        {
            if (interpolated->GetIsSelected())
            {
                added = true;
                interpolated->AddControlPoint(point);
                point = CreateRef<Point>(point->GetPosition(), point->GetName());
            }
        }

        return added;
    }

    void Scene::RenderBezier(Ref<BezierC0> bezierC0)
    {
        auto controlPoints = bezierC0->GetControlPoints();
        if (controlPoints.empty())
            return;


        std::vector<glm::vec3> controlPointsPositions;
        for (int i = 0; i < controlPoints.size(); i++)
        {
            auto point = controlPoints[i];
            auto color = point->GetIsSelected() ? Renderer::SELECTED_COLOR : Renderer::DEFAULT_COLOR;
            glm::vec3 position = point->GetTransform()->GetMatrix() * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
            Renderer::RenderPoint(position, color);

            controlPointsPositions.push_back(position);
        }


        if (bezierC0->GetShowPolygon())
        {
            for (int i = 0; i < controlPointsPositions.size() - 1; i++)
            {
                auto start = controlPointsPositions[i];
                auto end = controlPointsPositions[i + 1];
                Renderer::RenderLine(start, end, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
            }
        }

        auto bezierColor = bezierC0->GetIsSelected() ? Renderer::SELECTED_COLOR : Renderer::DEFAULT_COLOR;

        for (int i = 0; i < controlPointsPositions.size(); i += 3)
        {
            if (i + 3 < controlPointsPositions.size())
            {
                Renderer::ShaderRenderBezierC0(
                    controlPointsPositions[i],
                    controlPointsPositions[i + 1],
                    controlPointsPositions[i + 2],
                    controlPointsPositions[i + 3],
                    bezierColor);
            }
            else if (i + 2 < controlPointsPositions.size())
            {
                Renderer::ShaderRenderBezierC0(
                    controlPointsPositions[i],
                    controlPointsPositions[i + 1],
                    controlPointsPositions[i + 2],
                    bezierColor);
            }
            else if (i + 1 < controlPointsPositions.size())
            {
                Renderer::RenderLine(controlPointsPositions[i], controlPointsPositions[i + 1], bezierColor);
            }
        }
    }

    void Scene::RenderBSpline(Ref<BSpline> bSpline)
    {
        auto controlPoints = bSpline->GetControlPoints();
        if (controlPoints.size() < 4)
        {
            for (auto controlPoint : controlPoints)
            {
                auto color = controlPoint->GetIsSelected() ? Renderer::SELECTED_COLOR : Renderer::DEFAULT_COLOR;
                Renderer::RenderPoint(controlPoint->GetPosition(), color);
            }

            return;
        }

        auto bezierPoints = bSpline->GetBezierControlPoints();
        auto bSplineColor = bSpline->GetIsSelected() ? Renderer::SELECTED_COLOR : Renderer::DEFAULT_COLOR;

        for (int i = 0; i < bezierPoints.size(); i += 3)
        {
            if (i + 3 < bezierPoints.size())
            {
                Renderer::ShaderRenderBezierC0(
                    bezierPoints[i],
                    bezierPoints[i + 1],
                    bezierPoints[i + 2],
                    bezierPoints[i + 3],
                    bSplineColor);
            }
            else if (i + 2 < bezierPoints.size())
            {
                Renderer::ShaderRenderBezierC0(
                    bezierPoints[i],
                    bezierPoints[i + 1],
                    bezierPoints[i + 2],
                    bSplineColor);
            }
            else if (i + 1 < bezierPoints.size())
            {
                Renderer::RenderLine(bezierPoints[i], bezierPoints[i + 1], bSplineColor);
            }
        }

        for (auto controlPoint : controlPoints)
        {
            auto color = controlPoint->GetIsSelected() ? Renderer::SELECTED_COLOR : Renderer::DEFAULT_COLOR;
            Renderer::RenderPoint(controlPoint->GetPosition(), color);
        }

        if (bSpline->GetShowBSplinePolygon())
        {
            for (int i = 0; i < controlPoints.size() - 1; i++)
            {
                auto color = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
                Renderer::RenderLine(controlPoints[i]->GetPosition(), controlPoints[i + 1]->GetPosition(), color);
            }
        }

        if (bSpline->GetIsBezierBasis())
        {
            for (auto controlPoint : bezierPoints)
            {
                Renderer::RenderPoint(controlPoint, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
            }

            if (bSpline->GetShowBezierPolygon())
            {
                for (int i = 0; i < bezierPoints.size() - 1; i++)
                {
                    auto color = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
                    Renderer::RenderLine(bezierPoints[i], bezierPoints[i + 1], color);
                }
            }
        }
    }

    void Scene::RenderInterpolatedCurve(Ref<InterpolatedCurve> interPolatedCurve)
    {
        auto controlPoints = interPolatedCurve->GetControlPoints();
        auto bezierPoints = interPolatedCurve->GetBezierControlPoints();
        auto bSplineColor = interPolatedCurve->GetIsSelected() ? Renderer::SELECTED_COLOR : Renderer::DEFAULT_COLOR;

        for (int i = 0; i < bezierPoints.size(); i += 3)
        {
            if (i + 3 < bezierPoints.size())
            {
                Renderer::ShaderRenderBezierC0(
                    bezierPoints[i],
                    bezierPoints[i + 1],
                    bezierPoints[i + 2],
                    bezierPoints[i + 3],
                    bSplineColor);
            }
            else if (i + 2 < bezierPoints.size())
            {
                Renderer::ShaderRenderBezierC0(
                    bezierPoints[i],
                    bezierPoints[i + 1],
                    bezierPoints[i + 2],
                    bSplineColor);
            }
            else if (i + 1 < bezierPoints.size())
            {
                Renderer::RenderLine(bezierPoints[i], bezierPoints[i + 1], bSplineColor);
            }
        }

        for (auto controlPoint : controlPoints)
        {
            auto color = controlPoint->GetIsSelected() ? Renderer::SELECTED_COLOR : Renderer::DEFAULT_COLOR;
            Renderer::RenderPoint(controlPoint->GetPosition(), color);
        }

        if (interPolatedCurve->GetShowPolygon())
        {
            for (int i = 0; i < bezierPoints.size() - 1; i++)
            {
                auto color = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
                Renderer::RenderLine(bezierPoints[i], bezierPoints[i + 1], color);
            }
        }
    }

    void Scene::DeleteFreePoint(Ref<Point> point)
    {
        {
            auto it = std::find(m_Points.begin(), m_Points.end(), point);
            if (it != m_Points.end())
            {
                m_Points.erase(it);
            }
        }
        {
            auto it = std::find(m_FreePoints.begin(), m_FreePoints.end(), point);
            if (it != m_FreePoints.end())
            {
                m_FreePoints.erase(it);
            }
        }
    }

    void Scene::DeleteTorus(Ref<Torus> torus)
    {
        auto it = std::find(m_Torus.begin(), m_Torus.end(), torus);
        if (it != m_Torus.end())
        {
            m_Torus.erase(it);
        }
    }

    void Scene::DeleteBezierC0(Ref<BezierC0> bezierC0)
    {
        auto it = std::find(m_BezierC0.begin(), m_BezierC0.end(), bezierC0);
        if (it != m_BezierC0.end())
        {
            for (auto point : bezierC0->GetControlPoints())
            {
                m_FreePoints.push_back(point);
            }

            m_BezierC0.erase(it);
        }
    }

    void Scene::DeleteBSpline(Ref<BSpline> bSpline)
    {
        auto it = std::find(m_BSpline.begin(), m_BSpline.end(), bSpline);
        if (it != m_BSpline.end())
        {
            for (auto point : bSpline->GetControlPoints())
            {
                m_FreePoints.push_back(point);
            }

            m_BSpline.erase(it);
        }
    }

    void Scene::DeleteInterpolatedCurve(Ref<InterpolatedCurve> interpolatedCurve)
    {
        auto it = std::find(m_InterpolatedCurve.begin(), m_InterpolatedCurve.end(), interpolatedCurve);
        if (it != m_InterpolatedCurve.end())
        {
            for (auto point : interpolatedCurve->GetControlPoints())
            {
                m_FreePoints.push_back(point);
            }

            m_InterpolatedCurve.erase(it);
        }
    }
}