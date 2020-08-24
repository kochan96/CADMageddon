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

    void Scene::RemovePointFromBezier(Ref<BezierC0> bezier, Ref<Point> point)
    {
        m_FreePoints.push_back(point);
        bezier->RemoveControlPoint(point);
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
                Renderer::RenderBezierC0(
                    controlPointsPositions[i],
                    controlPointsPositions[i + 1],
                    controlPointsPositions[i + 2],
                    controlPointsPositions[i + 3],
                    bezierColor);
            }
            else if (i + 2 < controlPointsPositions.size())
            {
                Renderer::RenderBezierC0(
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
}