#pragma once
#include "Point.h"
#include "Torus.h"
#include "Core\Base.h"

namespace CADMageddon
{
    class Scene
    {
    public:
        Scene();

        Ref<Point> CreatePoint(glm::vec3 position, std::string name);
        Ref<Torus> CreateTorus(glm::vec3 position, std::string name);
        
        void Update();

        std::vector<Ref<Point>> GetFreePoints() { return m_FreePoints; }
        std::vector<Ref<Torus>> GetTorus() { return m_Torus; }
        std::vector<Ref<Point>> GetAllPoints();

    private:
        std::vector<Ref<Point>> m_FreePoints;
        std::vector<Ref<Torus>> m_Torus;

    };
}