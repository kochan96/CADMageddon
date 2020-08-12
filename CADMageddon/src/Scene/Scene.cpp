#include "Scene.h"
#include "Entity.h"
#include "Components.h"
#include "Rendering\Renderer.h"
#include "Core\Input.h"

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
        torusEntity.AddComponent<TransformComponent>().Translation = position;
        torusEntity.AddComponent<HierarchyComponent>();
        return torusEntity;
    }

    Entity Scene::CreatePointEntity(const glm::vec3& position, const std::string& name)
    {
        auto pointEntity = CreateEntity(name);
        pointEntity.AddComponent<PointComponent>().Point = Point(glm::vec3(0.0f, 0.0f, 0.0f), name);
        pointEntity.AddComponent<TransformComponent>().Translation = position;
        pointEntity.AddComponent<HierarchyComponent>();
        return pointEntity;
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

                Renderer::RenderTorus(points, torusComponent.Indices, transformComponent, color);
            }

            if (entity.HasComponent<PointComponent>() && entity.HasComponent<TransformComponent>())
            {
                auto& pointComponent = entity.GetComponent<PointComponent>();
                auto& transformComponent = entity.GetComponent<TransformComponent>();
                glm::vec3 position = transformComponent.GetMatrix() * glm::vec4(pointComponent.Point.GetPosition(), 1.0f);

                Renderer::RenderPoint(position, color);
            }
        }
    }
}