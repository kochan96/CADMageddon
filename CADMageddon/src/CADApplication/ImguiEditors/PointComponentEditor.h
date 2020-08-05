#pragma once
#include "cadpch.h"
#include "Scene\Components.h"
#include "imgui.h"

namespace CADMageddon
{
    void PointComponentEditor(PointComponent& component)
    {
        ImGui::BeginGroup();

        ImGui::Text("PointComponent");

        glm::vec3 position = component.Position;
        if (ImGui::DragFloat3("Position", &position.x, 0.1f))
        {
            component.Position = position;
        }

        ImGui::EndGroup();
    }
}
