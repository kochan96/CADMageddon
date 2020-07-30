#pragma once
#include <entt.hpp>
#include "Core/Timestep.h"

namespace CADMageddon
{

    class Entity;

    class Scene
    {
    public:
        Scene();
        ~Scene();

        Entity CreateEntity(const std::string& name = "Entity");
        Entity CreateTorusEntity(const std::string& name = "Torus");

        void OnUpdate(Timestep ts);

        std::vector<Entity> GetEntities() { return m_Entities; }

    private:
        std::vector<Entity> m_Entities;

        entt::registry m_Registry;

        friend class Entity;
    };
}