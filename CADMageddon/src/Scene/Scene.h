#pragma once
#include <entt.hpp>
#include "Core/Timestep.h"
#include "Core/Base.h"
#include <glm/glm.hpp>

namespace CADMageddon
{
    class Entity;
    class Point;
    struct BezierC0Component;

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
        Entity CreateBezierC0Entity(const std::string& name = "BezierC0");

        void OnUpdate(Timestep ts);

        std::vector<Entity> GetEntities() const { return m_Entities; }


    private:
        void RenderBezierC0(BezierC0Component& component, glm::vec4 color);
        void CheckPoint(Entity entity, Ref<Point> point);

        bool AddPointToSelectedBezierC0(Ref<Point>);

    private:
        std::vector<Entity> m_Entities;

        entt::registry m_Registry;

        friend class Entity;
    };
}