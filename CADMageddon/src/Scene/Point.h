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
        Point() : m_Name("Point")
        {
            m_Transform = CreateRef<Transform>();
        }

        Point(glm::vec3& position) : m_Name("Point")
        {
            m_Transform = CreateRef<Transform>();
            m_Transform->Translation = position;
        }

        Point(const glm::vec3& position, const std::string& name = "Point")
            : m_Name(name)
        {
            m_Transform = CreateRef<Transform>();
            m_Transform->Translation = position;
        }

        std::string GetName() const { return m_Name; }
        void SetName(const std::string& name) { m_Name = name; }

        Ref<Transform> GetTransform() { return m_Transform; }
        void SetTransform(Ref<Transform> transform) { m_Transform = transform; }

        bool GetIsSelected() const { return m_IsSelected; }
        void SetIsSelected(bool isSelected) { m_IsSelected = isSelected; }

        glm::vec3 GetPosition() { return m_Transform->GetMatrix() * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f); }

        bool GetIsVisible() const { return m_IsVisible; }
        void SetIsVisible(bool visible) { m_IsVisible = visible; }

        int GetReferencedCount() const { return m_ReferencedCount; }
        void SetReferencedCount(int referencedCount) { m_ReferencedCount = referencedCount; }

    private:
        Ref<Transform> m_Transform;
        std::string m_Name;
        glm::vec3 m_Position = { 0.0f,0.0f,0.0f };
        bool m_IsSelected = false;
        bool m_IsVisible = true;
        int m_ReferencedCount = 0;
    };
}

