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
        Ref<Transform> Parent = nullptr;

        glm::mat4 GetMatrix()
        {
            auto translationMatrix = glm::translate(glm::mat4(1.0f), Translation);
            auto rotationMatrix =
                glm::rotate(glm::mat4(1.0f), glm::radians(Rotation.x), glm::vec3(1.0f, 0.0f, 0.0f))
                * glm::rotate(glm::mat4(1.0f), glm::radians(Rotation.y), glm::vec3(0.0f, 1.0f, 0.0f))
                * glm::rotate(glm::mat4(1.0f), glm::radians(Rotation.z), glm::vec3(0.0f, 0.0f, -1.0f));
            auto scaleMatrix = glm::scale(glm::mat4(1.0f), Scale);

            auto modelMatrix = translationMatrix * rotationMatrix * scaleMatrix;

            if (Parent)
            {
                glm::vec3 parentTranslation = Parent->GetMatrix() * glm::vec4(Translation, 1.0f);
                parentTranslation -= Translation;
                modelMatrix = glm::translate(glm::mat4(1.0f), parentTranslation) * modelMatrix;
            }

            return modelMatrix;
        }
    };
}