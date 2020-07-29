#include "Scene.h"
#include "Entity.h"
#include "Components.h"

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
        entity.AddComponent<NameComponent>(name);
        return entity;
    }

    void Scene::OnUpdate(Timestep ts)
    {

    }
}