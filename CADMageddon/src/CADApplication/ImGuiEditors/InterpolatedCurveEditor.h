#pragma once
#include "Scene\InterpolatedCurve.h"
#include "Core\Base.h"
#include "imgui.h"
#include "misc\cpp\imgui_stdlib.h"

namespace CADMageddon
{
    void InterpolatedCurveEditor(Ref<InterpolatedCurve> interpolated)
    {
        ImGui::BeginGroup();

        auto name = interpolated->GetName();
        if (ImGui::InputText("Name", &name))
        {
            interpolated->SetName(name);
        }

        bool showPolygon = interpolated->GetShowPolygon();
        if (ImGui::Checkbox("ShowPolygon", &showPolygon))
        {
            interpolated->SetShowPolygon(showPolygon);
        }

        ImGui::EndGroup();
    }
}