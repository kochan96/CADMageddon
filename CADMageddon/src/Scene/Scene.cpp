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
        torusEntity.AddComponent<SceneSelectableComponent>();
        return torusEntity;
    }

    Entity Scene::CreatePointEntity(const glm::vec3& position, const std::string& name)
    {
        auto pointEntity = CreateEntity(name);
        pointEntity.AddComponent<PointComponent>().Position = position;
        pointEntity.AddComponent<HierarchyComponent>();
        pointEntity.AddComponent<SceneSelectableComponent>();
        return pointEntity;
    }

    void Scene::OnUpdate(Timestep ts)
    {
        for (auto entity : m_Entities)
        {
            auto color = Renderer::DEFAULT_COLOR;
            if (entity.HasComponent<SceneSelectableComponent>() && entity.GetComponent<SceneSelectableComponent>().IsSelected)
            {
                color = Renderer::SELECTED_COLOR;
            }

            if (entity.HasComponent<TorusComponent>() && entity.HasComponent<TransformComponent>())
            {
                auto& torusComponent = entity.GetComponent<TorusComponent>();
                auto& transformComponent = entity.GetComponent<TransformComponent>();

                Renderer::RenderTorus(torusComponent.Mesh, transformComponent, color);
            }

            if (entity.HasComponent<PointComponent>())
            {
                auto& pointComponent = entity.GetComponent<PointComponent>();
                Renderer::RenderPoint(pointComponent.Position, color);
            }
        }
    }
}