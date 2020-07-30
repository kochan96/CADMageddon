#include "Scene.h"
#include "Entity.h"
#include "Components.h"
#include "Rendering\Renderer.h"

namespace CADMageddon
{
    Scene::Scene()
    {

    }

    Scene::~Scene()
    {
    }

    Entity Scene::CreateEntity(const std::string& name)
    {
        Entity entity = { m_Registry.create(),this };
        
        m_Entities.push_back(entity);
        entity.AddComponent<NameComponent>(name);
        
        return entity;
    }

    Entity Scene::CreateTorusEntity(const std::string& name)
    {
        auto torusEntity = CreateEntity("Torus");
        torusEntity.AddComponent<TorusComponent>();
        torusEntity.AddComponent<TransformComponent>();
        torusEntity.AddComponent<ColorComponent>(glm::vec4(0.05f, 0.08f, 0.71f, 1.0f));
        return torusEntity;
    }

    void Scene::OnUpdate(Timestep ts)
    {
        auto view = m_Registry.group<TorusComponent, TransformComponent, ColorComponent>();

        for (auto entity : view)
        {
            auto& [torusComponent, transformComponent, colorComponent] = view.get<TorusComponent, TransformComponent, ColorComponent>(entity);
            Renderer::RenderTorus(torusComponent.Mesh, transformComponent, colorComponent.Color);
        }
    }
}