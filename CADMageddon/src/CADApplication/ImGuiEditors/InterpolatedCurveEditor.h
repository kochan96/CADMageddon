#pragma once
#include "Scene\InterpolatedCurve.h"
#include "BaseObjectEditor.h"
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

        auto visible = interpolated->GetShowPoints();
        if (ImGui::Checkbox("ShowPoints", &visible))
        {
            interpolated->SetShowPoints(visible);
        }

        bool showPolygon = interpolated->GetShowPolygon();
        if (ImGui::Checkbox("ShowPolygon", &showPolygon))
        {
            interpolated->SetShowPolygon(showPolygon);
        }

        ImGui::EndGroup();

        BaseObjectEditor(interpolated);
    }
}