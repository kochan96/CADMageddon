#pragma once
#include "Scene\BezierPatch.h"
#include "Core\Base.h"
#include "imgui.h"
#include "misc\cpp\imgui_stdlib.h"

namespace CADMageddon
{
    void BezierPatchEditor(Ref<BezierPatch> bezierPatch)
    {
        ImGui::BeginGroup();

        auto name = bezierPatch->GetName();
        if (ImGui::InputText("Name", &name))
        {
            bezierPatch->SetName(name);
        }

        auto visible = bezierPatch->GetShowPoints();
        if (ImGui::Checkbox("ShowPoints", &visible))
        {
            bezierPatch->SetShowPoints(visible);
        }

        bool showPolygon = bezierPatch->GetShowPolygon();
        if (ImGui::Checkbox("ShowPolygon", &showPolygon))
        {
            bezierPatch->SetShowPolygon(showPolygon);
        }

        int uDivision = bezierPatch->GetUDivisionCount();

        if (ImGui::DragInt("UDivision", &uDivision, 1.0f, 1, 64))
        {
            bezierPatch->SetUDivisionCount(uDivision);
        }

        int vDivision = bezierPatch->GetVDivisionCount();

        if (ImGui::DragInt("VDivision", &vDivision, 1.0f, 1, 64))
        {
            bezierPatch->SetVDivisionCount(vDivision);
        }

        ImGui::Text("PatchCountX %d", bezierPatch->GetPatchCountX());
        ImGui::Text("PatchCountY %d", bezierPatch->GetPatchCountY());
        ImGui::Text("Cylinder: %s", bezierPatch->GetIsCylinder() ? "Yes" : "No");

        ImGui::EndGroup();
    }
}