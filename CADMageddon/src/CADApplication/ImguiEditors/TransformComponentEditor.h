#pragma once
#include "Scene\Components.h"
#include "imgui.h"

namespace CADMageddon
{
    void TransformComponentEditor(TransformComponent& component)
    {
        ImGui::BeginGroup();

        ImGui::Text("TransformComponent");

        glm::vec3 translation = component.Translation;
        if (ImGui::DragFloat3("Position", &translation.x, 0.1f))
        {
            component.Translation = translation;
        }

        glm::vec3 rotation = component.Rotation;
        if (ImGui::DragFloat3("Rotation", &rotation.x, 1.0f, -90.0f, 90.0f))
        {
            component.Rotation = rotation;
        }

        glm::vec3 scale = component.Scale;
        if (ImGui::DragFloat3("Scale", &scale.x, 0.1f))
        {
            component.Scale = scale;
        }

        ImGui::EndGroup();
    }
}