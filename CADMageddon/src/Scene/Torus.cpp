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

    glm::vec3 Torus::GetPointAt(float u, float v)
    {
        u = std::clamp(u, GetMinU(), GetMaxU());
        v = std::clamp(v, GetMinV(), GetMaxV());

        glm::vec3 point;

        point.x = (m_TorusParameters.MajorRadius + m_TorusParameters.MinorRadius * cos(v)) * cos(u);
        point.y = (m_TorusParameters.MajorRadius + m_TorusParameters.MinorRadius * cos(v)) * sin(u);
        point.z = m_TorusParameters.MinorRadius * sin(v);

        return m_Transform->GetMatrix() * glm::vec4(point, 1.0f);
    }

    glm::vec3 Torus::GetTangentUAt(float u, float v)
    {
        u = std::clamp(u, GetMinU(), GetMaxU());
        v = std::clamp(v, GetMinV(), GetMaxV());

        glm::vec3 point;

        point.x = (m_TorusParameters.MajorRadius + m_TorusParameters.MinorRadius * cos(v)) * -sin(u);
        point.y = (m_TorusParameters.MajorRadius + m_TorusParameters.MinorRadius * cos(v)) * cos(u);
        point.z = 0;

        return m_Transform->GetMatrix() * glm::vec4(point, 0.0f);
    }

    glm::vec3 Torus::GetTangentVAt(float u, float v)
    {
        u = std::clamp(u, GetMinU(), GetMaxU());
        v = std::clamp(v, GetMinV(), GetMaxV());

        glm::vec3 point;

        point.x = m_TorusParameters.MinorRadius * -sin(v) * cos(u);
        point.y = m_TorusParameters.MinorRadius * -sin(v) * sin(u);
        point.z = m_TorusParameters.MinorRadius * cos(v);

        return m_Transform->GetMatrix() * glm::vec4(point, 0.0f);
    }

    float Torus::GetMinU() const
    {
        return 0.0f;
    }

    float Torus::GetMaxU() const
    {
        return glm::two_pi<float>();
    }

    float Torus::GetMinV() const
    {
        return 0.0f;
    }

    float Torus::GetMaxV() const
    {
        return glm::two_pi<float>();
    }
}


