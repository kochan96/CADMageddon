#pragma once
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

namespace CADMageddon
{
    struct Transform
    {
        glm::vec3 Translation = { 0.0f,0.0f,0.0f };
        glm::vec3 Rotation = { 0.0f,0.0f,0.0f };
        glm::vec3 Scale = { 1.0f,1.0f,1.0f };

        glm::mat4 GetMatrix()
        {
            auto translationMatrix = glm::translate(glm::mat4(1.0f), Translation);
            auto rotationMatrix =
                glm::rotate(glm::mat4(1.0f), Rotation.x, glm::vec3(1.0f, 0.0f, 0.0f))
                * glm::rotate(glm::mat4(1.0f), Rotation.y, glm::vec3(0.0f, 1.0f, 0.0f))
                * glm::rotate(glm::mat4(1.0f), Rotation.z, glm::vec3(0.0f, 0.0f, -1.0f));
            auto scaleMatrix = glm::scale(glm::mat4(1.0f), Scale);

            return translationMatrix * rotationMatrix * scaleMatrix;
        }
    };
}