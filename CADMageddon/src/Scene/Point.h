#pragma once
#include "cadpch.h"
#include "Core\Base.h"
#include <glm\glm.hpp>
#include <glm\gtc\matrix_inverse.hpp>
#include "Transform.h"
#include "Core\Base.h"

namespace CADMageddon
{
    class Point
    {
    public:
        Point() :m_Position(0.0f, 0.0f, 0.0f), m_Name("Point")
        {
            m_Transform = CreateRef<Transform>();
        }

        Point(glm::vec3& position) :m_Position(position), m_Name("Point")
        {
            m_Transform = CreateRef<Transform>();
        }

        Point(const glm::vec3& position, const std::string& name = "Point")
            :m_Position(position), m_Name(name)
        {
            m_Transform = CreateRef<Transform>();
        }

        std::string GetName() const { return m_Name; }
        void SetName(const std::string& name) { m_Name = name; }

        glm::vec3 GetPosition() const { return m_Position; }
        void SetPosition(const glm::vec3& position) { m_Position = position; }

        glm::vec3 GetWorldPositon() const { return m_Transform->GetMatrix() * glm::vec4(m_Position, 1.0f); }
        void SetWorldPosition(glm::vec3 worldPosition) { m_Position = glm::affineInverse(m_Transform->GetMatrix()) * glm::vec4(worldPosition, 1.0f); }

        Ref<Transform> GetTransform() { return m_Transform; }
        void SetTransform(Ref<Transform> transform) { m_Transform = transform; }

    private:
        Ref<Transform> m_Transform;
        std::string m_Name;
        glm::vec3 m_Position;
    };
}

