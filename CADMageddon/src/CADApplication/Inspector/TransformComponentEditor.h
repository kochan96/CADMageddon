#pragma once
#include <glm/gtx/quaternion.hpp>
#include <glm\gtx\matrix_decompose.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Scene\Components.h"
#include "imgui.h"

namespace CADMageddon
{
    std::tuple<glm::vec3, glm::quat, glm::vec3> GetTransformDecomposition(const glm::mat4& transform)
    {
        glm::vec3 scale, translation, skew;
        glm::vec4 perspective;
        glm::quat orientation;

        glm::decompose(transform, scale, orientation, translation, skew, perspective);

        return { translation, orientation, scale };
    }

    void TransformComponentEditor(TransformComponent& component)
    {
        ImGui::BeginGroup();

        ImGui::Text("TransformComponent");

        auto [translation, rotationQuat, scale] = GetTransformDecomposition(component);
        glm::vec3 rotation = glm::degrees(glm::eulerAngles(rotationQuat));

        ImGui::Text("Translation");
        ImGui::SameLine();

        bool updateTransform = false;

        if (ImGui::DragFloat3("##translation", glm::value_ptr(translation), 0.25f))
        {
            //tc.Transform[3] = glm::vec4(translation, 1.0f);
            updateTransform = true;
        }

        ImGui::Text("Rotation");
        ImGui::SameLine();

        if (ImGui::DragFloat3("##rotation", glm::value_ptr(rotation), 0.25f))
        {
            updateTransform = true;
            // tc.Transform[3] = glm::vec4(translation, 1.0f);
        }

        ImGui::Text("Scale");
        ImGui::SameLine();

        if (ImGui::DragFloat3("##scale", glm::value_ptr(scale), 0.25f))
        {
            updateTransform = true;
        }

        if (updateTransform)
        {
            LOG_INFO("ROTATION {} {} {}", rotation.x, rotation.y, rotation.z);
            component.Transform = glm::translate(glm::mat4(1.0f), translation)
                * glm::toMat4(glm::quat(glm::radians(rotation)))
                * glm::scale(glm::mat4(1.0f), scale);
        }

        ImGui::EndGroup();
    }
}