#pragma once
#include "cadpch.h"
#include "Point.h"
#include "Core\Base.h"

namespace CADMageddon
{
    struct TorusParameters
    {
        float MajorRadius = 1.0f;
        float MinorRadius = 0.3f;
        int MajorRadiusCount = 30;
        int MinorRadiusCount = 15;
    };

    class Torus
    {
    public:
        Torus(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), std::string name = "Torus");

        std::string GetName() const { return m_Name; }
        void SetName(std::string name) { m_Name = name; }

        TorusParameters& GetTorusParameters() { return m_TorusParameters; }

        std::vector<Ref<Point>> GetPoints() { return m_Points; }
        std::vector<uint32_t> GetIndices() { return m_Indices; }
        Ref<Transform> GetTransform() { return m_Transform; }

        void RecalculateMesh();

        bool GetIsSelected() { return m_IsSelected; }
        void SetIsSelected(bool isSelected) { m_IsSelected = isSelected; }

    private:
        Ref<Transform> m_Transform;
        std::string m_Name;
        TorusParameters m_TorusParameters;

        std::vector<Ref<Point>> m_Points;
        std::vector<uint32_t> m_Indices;

        bool m_IsSelected = false;
    };

}