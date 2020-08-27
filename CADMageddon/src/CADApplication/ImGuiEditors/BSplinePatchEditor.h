#pragma once
#include "Scene\BSplinePatch.h"
#include "Core\Base.h"
#include "imgui.h"
#include "misc\cpp\imgui_stdlib.h"

namespace CADMageddon
{
    void BSplinePatchEditor(Ref<BSplinePatch> bSplinePatch)
    {
        ImGui::BeginGroup();

        auto name = bSplinePatch->GetName();
        if (ImGui::InputText("Name", &name))
        {
            bSplinePatch->SetName(name);
        }

        auto visible = bSplinePatch->GetShowPoints();
        if (ImGui::Checkbox("ShowPoints", &visible))
        {
            bSplinePatch->SetShowPoints(visible);
        }

        bool showPolygon = bSplinePatch->GetShowPolygon();
        if (ImGui::Checkbox("ShowPolygon", &showPolygon))
        {
            bSplinePatch->SetShowPolygon(showPolygon);
        }

        int uDivision = bSplinePatch->GetUDivisionCount();

        if (ImGui::DragInt("UDivision", &uDivision, 1.0f, 1, 64))
        {
            bSplinePatch->SetUDivisionCount(uDivision);
        }

        int vDivision = bSplinePatch->GetVDivisionCount();

        if (ImGui::DragInt("VDivision", &vDivision, 1.0f, 1, 64))
        {
            bSplinePatch->SetVDivisionCount(vDivision);
        }

        ImGui::Text("PatchCountX %d", bSplinePatch->GetPatchCountX());
        ImGui::Text("PatchCountY %d", bSplinePatch->GetPatchCountY());
        ImGui::Text("Cylinder: %s", bSplinePatch->GetIsCylinder() ? "Yes" : "No");


        ImGui::EndGroup();
    }
}