#pragma once
#include "cadpch.h"
#include <glm\glm.hpp>

namespace CADMageddon
{
    class Point
    {
    public:
        Point() :m_Position(0.0f, 0.0f, 0.0f), m_Name("Point") {}
        Point(glm::vec3& position) :m_Position(position), m_Name("Point") {}
        Point(const glm::vec3& position, const std::string& name = "Point")
            :m_Position(position), m_Name(name) {}

        std::string GetName() const { return m_Name; }
        void SetName(const std::string& name) { m_Name = name; }

        glm::vec3 GetPosition() const { return m_Position; }
        void SetPosition(const glm::vec3& position) { m_Position = position; }

    private:
        std::string m_Name;
        glm::vec3 m_Position;
    };
}

