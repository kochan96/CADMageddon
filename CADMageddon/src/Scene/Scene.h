#pragma once
#include <entt.hpp>
#include "Core/Timestep.h"
#include <glm/glm.hpp>

namespace CADMageddon
{
    class Entity;

    class Scene
    {
    public:
        Scene();
        ~Scene();

        void DestroyEntity(Entity& entity);
        void DestroySelected();

        Entity CreateEntity(const std::string& name = "Entity");
        Entity CreateTorusEntity(const glm::vec3& position, const std::string& name = "Torus");
        Entity CreatePointEntity(const glm::vec3& position, const std::string& name = "Point");

        void OnUpdate(Timestep ts);

        std::vector<Entity> GetEntities() const { return m_Entities; }

    private:
        std::vector<Entity> m_Entities;

        entt::registry m_Registry;

        friend class Entity;
    };
}