#pragma once
#include "cadpch.h"
#include "Scene\Components.h"
#include "imgui.h"
#include "misc\cpp\imgui_stdlib.h"

namespace CADMageddon
{
    void NameComponentEditor(NameComponent& component)
    {
        ImGui::BeginGroup();
        ImGui::Text("NameComponent");

        ImGui::InputText("Name", &component.Name);

        ImGui::EndGroup();
    }
}