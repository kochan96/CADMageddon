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
        void Update();
        void DeleteSelected();

        Ref<Point> CreatePoint(glm::vec3 position, std::string name);
        Ref<Torus> CreateTorus(glm::vec3 position, std::string name);
        
        std::vector<Ref<Point>> GetFreePoints() const { return m_FreePoints; }
        std::vector<Ref<Torus>> GetTorus() const { return m_Torus; }
        std::vector<Ref<Point>> GetPoints() const { return m_Points; }

    private:
        void DeleteFreePoint(Ref<Point> point);
        void DeleteTorus(Ref<Torus> torus);

    private:
        std::vector<Ref<Point>> m_FreePoints;
        std::vector<Ref<Point>> m_Points;
        std::vector<Ref<Torus>> m_Torus;

    };
}