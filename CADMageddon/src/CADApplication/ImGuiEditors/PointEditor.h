#pragma once
#include "Scene\Point.h"
#include "Core\Base.h"
#include "imgui.h"
#include "misc\cpp\imgui_stdlib.h"

namespace CADMageddon
{
    void PointEditor(Ref<Point> point)
    {
        ImGui::BeginGroup();

        auto name = point->GetName();
        if (ImGui::InputText("Name", &name))
        {
            point->SetName(name);
        }

        auto visible = point->GetIsVisible();
        if(ImGui::Checkbox("IsVisible",&visible))
        {
            point->SetIsVisible(visible);
        }

        auto position = point->GetTransform()->Translation;
        if (ImGui::DragFloat3("Position", &position.x, 0.1f))
        {
            point->GetTransform()->Translation = position;
        }

        

        ImGui::EndGroup();
    }
}