#pragma once
#include "Scene\Components.h"
#include "imgui.h"

namespace CADMageddon
{
    void TransformComponentEditor(TransformComponent& component)
    {
        ImGui::BeginGroup();

        ImGui::Text("TransformComponent");

        auto transform = component.Transform;

        glm::vec3 translation = transform->Translation;
        if (ImGui::DragFloat3("Position", &translation.x, 0.1f))
        {
            transform->Translation = translation;
        }

        glm::vec3 rotation = transform->Rotation;
        if (ImGui::DragFloat3("Rotation", &rotation.x, 1.0f, -90.0f, 90.0f))
        {
            transform->Rotation = rotation;
        }

        glm::vec3 scale = transform->Scale;
        if (ImGui::DragFloat3("Scale", &scale.x, 0.1f))
        {
            transform->Scale = scale;
        }

        ImGui::EndGroup();
    }
}