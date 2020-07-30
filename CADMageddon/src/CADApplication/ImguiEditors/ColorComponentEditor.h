#pragma once
#include "Scene\Components.h"
#include "imgui.h"

namespace CADMageddon
{
    void ColorComponentEditor(ColorComponent& colorComponent)
    {
        ImGui::BeginGroup();

        auto color = colorComponent.Color;

        if (ImGui::ColorEdit4("Color", &color.x))
        {
            colorComponent.Color = color;
        }

        if (ImGui::ColorPicker4("ColorPicker", &color.x))
        {
            colorComponent.Color = color;
        }

        ImGui::EndGroup();
    }

}