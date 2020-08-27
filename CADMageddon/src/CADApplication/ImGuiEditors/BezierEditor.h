#pragma once
#include "Scene\BezierC0.h"
#include "Core\Base.h"
#include "imgui.h"
#include "misc\cpp\imgui_stdlib.h"

namespace CADMageddon
{
    void BezierEditor(Ref<BezierC0> bezier)
    {
        ImGui::BeginGroup();

        auto name = bezier->GetName();
        if (ImGui::InputText("Name", &name))
        {
            bezier->SetName(name);
        }

        auto visible = bezier->GetShowPoints();
        if (ImGui::Checkbox("ShowPoints", &visible))
        {
            bezier->SetShowPoints(visible);
        }

        bool showPolygon = bezier->GetShowPolygon();
        if (ImGui::Checkbox("ShowPolygon", &showPolygon))
        {
            bezier->SetShowPolygon(showPolygon);
        }
        

        ImGui::EndGroup();
    }
}