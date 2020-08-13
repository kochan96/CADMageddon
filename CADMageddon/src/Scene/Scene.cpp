#include "Scene.h"
#include "Entity.h"
#include "Components.h"
#include "Rendering\Renderer.h"
#include "Core\Input.h"
#include "Point.h"

namespace CADMageddon
{
    Scene::Scene()
    {
    }

    Scene::~Scene()
    {
    }

    void Scene::DestroyEntity(Entity& entity)
    {
        auto it = std::find(m_Entities.begin(), m_Entities.end(), entity);

        if (it == m_Entities.end())
        {
            return;
        }

        if (it->HasComponent<PointComponent>())
        {
            CheckPoint(*it, it->GetComponent<PointComponent>().Point);
        }

        m_Entities.erase(it);
        m_Registry.destroy(entity.m_EntityHandle);
    }

    void Scene::DestroySelected()
    {
        std::vector<Entity> selectedEntities;

        for (auto entity : m_Entities)
        {
            if (entity.HasComponent<HierarchyComponent>())
            {
                if (entity.GetComponent<HierarchyComponent>().IsSelected)
                {
                    selectedEntities.push_back(entity);
                }
            }
        }

        for (auto entity : selectedEntities)
        {
            DestroyEntity(entity);
        }
    }

    Entity Scene::CreateEntity(const std::string& name)
    {
        Entity entity = { m_Registry.create(),this };

        m_Entities.push_back(entity);
        entity.AddComponent<NameComponent>(name);

        return entity;
    }


    Entity Scene::CreateTorusEntity(const glm::vec3& position, const std::string& name)
    {
        auto torusEntity = CreateEntity(name);
        torusEntity.AddComponent<TorusComponent>();
        torusEntity.AddComponent<TransformComponent>().Transform->Translation = position;
        torusEntity.AddComponent<HierarchyComponent>();
        return torusEntity;
    }

    Entity Scene::CreatePointEntity(const glm::vec3& position, const std::string& name)
    {
        auto pointEntity = CreateEntity(name);
        auto transform = pointEntity.AddComponent<TransformComponent>().Transform;
        transform->Translation = position;

        auto point = pointEntity.AddComponent<PointComponent>(name, position).Point;
        point->GetTransform()->SetParent(transform);

        pointEntity.AddComponent<HierarchyComponent>();

        if (AddPointToSelectedBezierC0(point))
        {
        }

        return pointEntity;
    }

    Entity Scene::CreateBezierC0Entity(const std::string& name)
    {
        auto bezierEntity = CreateEntity(name);
        bezierEntity.AddComponent<HierarchyComponent>();
        bezierEntity.AddComponent<BezierC0Component>();
        return bezierEntity;
    }

    void Scene::OnUpdate(Timestep ts)
    {
        for (auto entity : m_Entities)
        {
            auto color = Renderer::DEFAULT_COLOR;

            if (entity.HasComponent<HierarchyComponent>())
            {
                if (entity.GetComponent<HierarchyComponent>().IsSelected)
                {
                    color = Renderer::SELECTED_COLOR;
                }
            }

            if (entity.HasComponent<TorusComponent>() && entity.HasComponent<TransformComponent>())
            {
                auto& torusComponent = entity.GetComponent<TorusComponent>();
                auto& transformComponent = entity.GetComponent<TransformComponent>();

                std::vector<glm::vec3> points;
                for (auto point : torusComponent.Points)
                {
                    points.push_back(point.GetPosition());
                }

                Renderer::RenderTorus(points, torusComponent.Indices, *transformComponent.Transform, color);
            }

            if (entity.HasComponent<PointComponent>())
            {
                auto& pointComponent = entity.GetComponent<PointComponent>();
                auto& transformComponent = entity.GetComponent<TransformComponent>();
                glm::vec3 position = glm::vec4(pointComponent.Point->GetWorldPositon(), 1.0f);

                Renderer::RenderPoint(position, color);
            }

            if (entity.HasComponent<BezierC0Component>())
            {
                auto& bezierC0 = entity.GetComponent<BezierC0Component>();
                RenderBezierC0(bezierC0, color);
            }
        }
    }

    void Scene::RenderBezierC0(BezierC0Component& component, glm::vec4 color)
    {
        if (component.ControlPoints.empty())
        {
            return;
        }

        auto controlPoints = component.ControlPoints;

        if (component.ShowPolygon)
        {
            for (unsigned int i = 0; i + 1 < controlPoints.size(); i++)
            {
                Renderer::RenderLine(controlPoints[i]->GetWorldPositon(), controlPoints[(i + 1)]->GetWorldPositon(), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
            }
        }

        for (unsigned int i = 0; i + 1 < controlPoints.size(); i += 3)
        {
            if ((i + 2) >= controlPoints.size())
            {
                Renderer::RenderLine(controlPoints[i]->GetWorldPositon(), controlPoints[(i + 1)]->GetWorldPositon(), color);
            }
            else if ((i + 3) >= controlPoints.size())
            {
                Renderer::RenderBezierC0(
                    controlPoints[i]->GetWorldPositon(),
                    controlPoints[(i + 1)]->GetWorldPositon(),
                    controlPoints[(i + 2)]->GetWorldPositon(),
                    100, color);
            }
            else
            {
                Renderer::RenderBezierC0(
                    controlPoints[i]->GetWorldPositon(),
                    controlPoints[(i + 1)]->GetWorldPositon(),
                    controlPoints[(i + 2)]->GetWorldPositon(),
                    controlPoints[(i + 3)]->GetWorldPositon(),
                    100, color);
            }
        }
    }

    void Scene::CheckPoint(Entity entity, Ref<Point> point)
    {
        for (auto entity : m_Entities)
        {
            if (entity.HasComponent<BezierC0Component>())
            {
                auto& component = entity.GetComponent<BezierC0Component>();
                auto it = std::find(component.ControlPoints.begin(), component.ControlPoints.end(), point);
                if (it != component.ControlPoints.end())
                    component.ControlPoints.erase(it);
            }
        }
    }

    bool Scene::AddPointToSelectedBezierC0(Ref<Point> point)
    {
        for (auto entity : m_Entities)
        {
            if (entity.HasComponent<BezierC0Component>() && entity.HasComponent<HierarchyComponent>() && entity.GetComponent<HierarchyComponent>().IsSelected)
            {
                entity.GetComponent<BezierC0Component>().ControlPoints.push_back(point);
                return true;
            }
        }

        return false;
    }
}