#pragma once
#include "cadpch.h"
#include <glm/glm.hpp>
#include "Scene/Mesh.h"
#include "Core/Base.h"
#include "Rendering\VertexArray.h"

namespace CADMageddon
{
    class Cursor3D
    {
    public:
        Cursor3D() = default;

        glm::vec2 getScreenPosition() const { return m_ScreenPosition; }
        void setScreenPosition(const glm::vec2& screenPosition) { m_ScreenPosition = screenPosition; }

        glm::vec3 getPosition() const { return m_Position; }
        void setPosition(const glm::vec3& position) { m_Position = position; }

    private:
        glm::vec3 m_Position = { 0.0f,0.0f,0.0f };
        glm::vec2 m_ScreenPosition = { 0.0f,0.0f };
    };
}