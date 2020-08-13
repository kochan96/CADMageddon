#pragma once
#include "cadpch.h"

namespace CADMageddon
{
    class Entity;

    class InspectorPanel
    {
    public:
        void Render();
        void Add(Entity entity);
        void Remove(Entity entity);
        void Clear();

    private:
        std::vector<Entity> m_Entities;

    };
}