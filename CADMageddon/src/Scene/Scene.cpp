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
                vertices.push_back(point->GetPosition());
            }

            auto color = torus->GetIsSelected() ? Renderer::SELECTED_COLOR : Renderer::DEFAULT_COLOR;
            Renderer::RenderTorus(vertices, torus->GetIndices(), torus->GetTransform()->GetMatrix(), color);
        }


        for (auto point : m_FreePoints)
        {
            auto color = point->GetIsSelected() ? Renderer::SELECTED_COLOR : Renderer::DEFAULT_COLOR;
            Renderer::RenderPoint(point->GetWorldPositon(), color);
        }
    }

    std::vector<Ref<Point>> Scene::GetAllPoints()
    {
        std::vector<Ref<Point>> points = m_FreePoints;

        for (auto torus : m_Torus)
        {
            auto torusPoints = torus->GetPoints();
            points.insert(points.end(), torusPoints.begin(), torusPoints.end());
        }


        return points;
    }
}