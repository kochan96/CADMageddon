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
        m_FreePoints.push_back(point);
        m_Points.push_back(point);
        return point;
    }

    Ref<Torus> Scene::CreateTorus(glm::vec3 position, std::string name)
    {
        auto torus = CreateRef<Torus>(position, name);
        m_Torus.push_back(torus);
        return torus;
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

        for (auto point : pointsToDelete)
        {
            DeleteFreePoint(point);
        }

        for (auto torus : torusToDelete)
        {
            DeleteTorus(torus);
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
}