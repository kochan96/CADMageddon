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

        m_TextureCoordinates.clear();
        m_TextureCoordinates.reserve(m_Points.size());

        int uCount = m_TorusParameters.MajorRadiusCount;
        int vCount = m_TorusParameters.MinorRadiusCount;
        float uDelta = 1.0f / uCount;
        float vDelta = 1.0f / vCount;
        for (int i = 0; i < uCount; i++)
        {
            float u = i * uDelta;
            for (int j = 0; j < vCount; j++)
            {
                float v = j * vDelta;
                m_TextureCoordinates.push_back(glm::vec2(u, v));
            }
        }
    }

    glm::vec3 Torus::GetPointAt(float u, float v)
    {
        u = glm::two_pi<float>() * u;
        v = glm::two_pi<float>() * v;

        glm::vec3 point;

        point.x = (m_TorusParameters.MajorRadius + m_TorusParameters.MinorRadius * cos(v)) * cos(u);
        point.y = (m_TorusParameters.MajorRadius + m_TorusParameters.MinorRadius * cos(v)) * sin(u);
        point.z = m_TorusParameters.MinorRadius * sin(v);

        return m_Transform->GetMatrix() * glm::vec4(point, 1.0f);
    }

    glm::vec3 Torus::GetTangentUAt(float u, float v)
    {
        u = glm::two_pi<float>() * u;
        v = glm::two_pi<float>() * v;

        glm::vec3 point;

        point.x = (m_TorusParameters.MajorRadius + m_TorusParameters.MinorRadius * cos(v)) * -sin(u);
        point.y = (m_TorusParameters.MajorRadius + m_TorusParameters.MinorRadius * cos(v)) * cos(u);
        point.z = 0;

        point *= glm::two_pi<float>();

        return m_Transform->GetMatrix() * glm::vec4(point, 0.0f);
    }

    glm::vec3 Torus::GetTangentVAt(float u, float v)
    {
        u = glm::two_pi<float>() * u;
        v = glm::two_pi<float>() * v;

        glm::vec3 point;

        point.x = m_TorusParameters.MinorRadius * -sin(v) * cos(u);
        point.y = m_TorusParameters.MinorRadius * -sin(v) * sin(u);
        point.z = m_TorusParameters.MinorRadius * cos(v);

        point *= glm::two_pi<float>();

        return m_Transform->GetMatrix() * glm::vec4(point, 0.0f);
    }

    float Torus::GetMinU() const
    {
        return 0.0f;
    }

    float Torus::GetMaxU() const
    {
        return 1.0f;
    }

    float Torus::GetMinV() const
    {
        return 0.0f;
    }

    float Torus::GetMaxV() const
    {
        return 1.0f;
    }
}


