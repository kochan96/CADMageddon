#include "Torus.h"
#include "ObjectFactory.h"

namespace CADMageddon
{
    Torus::Torus(glm::vec3 position, std::string name)
    {
        SetName(name);
        RecalculateMesh();
        m_Transform = CreateRef<Transform>();
        m_Transform->Translation = position;
    }

    void Torus::RecalculateMesh()
    {
        auto mesh = ObjectFactory::CreateTorusMesh(m_TorusParameters.MajorRadius, m_TorusParameters.MinorRadius, m_TorusParameters.MajorRadiusCount, m_TorusParameters.MinorRadiusCount);
        m_Points.clear();
        for (auto vertex : mesh.Vertices)
        {
            auto point = CreateRef<Point>(vertex, "Point");
            point->GetTransform()->Parent = m_Transform;
            m_Points.push_back(point);
        }

        m_Indices = mesh.Indices;
    }
}


