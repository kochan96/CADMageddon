#pragma once
#include "imgui.h"
#include "misc\cpp\imgui_stdlib.h"
#include "Scene/GregoryPatch.h"

namespace CADMageddon
{
    void GregoryEditor(Ref<GregoryPatch> gregoryPatch)
    {
        ImGui::BeginGroup();

        auto name = gregoryPatch->GetName();
        if (ImGui::InputText("Name", &name))
        {
            gregoryPatch->SetName(name);
        }

        bool showFirst = gregoryPatch->GetShowFirst();
        if (ImGui::Checkbox("Show First", &showFirst))
        {
            gregoryPatch->SetShowFirst(showFirst);
        }

        bool showFirstMesh = gregoryPatch->GetShowFirstMesh();
        if (ImGui::Checkbox("Show First Mesh", &showFirstMesh))
        {
            gregoryPatch->SetShowFirstMesh(showFirstMesh);
        }

        bool showSecond = gregoryPatch->GetShowSecond();
        if (ImGui::Checkbox("Show Second", &showSecond))
        {
            gregoryPatch->SetShowSecond(showSecond);
        }

        bool showSecondMesh = gregoryPatch->GetShowSecondMesh();
        if (ImGui::Checkbox("Show Second Mesh", &showSecondMesh))
        {
            gregoryPatch->SetShowSecondMesh(showSecondMesh);
        }

        bool showThird = gregoryPatch->GetShowThird();
        if (ImGui::Checkbox("Show Third", &showThird))
        {
            gregoryPatch->SetShowThird(showThird);
        }

        bool showThirdMesh = gregoryPatch->GetShowThirdMesh();
        if (ImGui::Checkbox("Show Tihrd Mesh", &showThirdMesh))
        {
            gregoryPatch->SetShowThirdMesh(showThirdMesh);
        }

        int uDivision = gregoryPatch->GetUDivisionCount();

        if (ImGui::DragInt("UDivision", &uDivision, 1.0f, 2, 64))
        {
            uDivision = std::clamp(uDivision, 2, 64);
            gregoryPatch->SetUDivisionCount(uDivision);
        }

        int vDivision = gregoryPatch->GetVDivisionCount();

        if (ImGui::DragInt("VDivision", &vDivision, 1.0f, 2, 64))
        {
            vDivision = std::clamp(vDivision, 2, 64);
            gregoryPatch->SetVDivisionCount(vDivision);
        }


        ImGui::EndGroup();
    }
}