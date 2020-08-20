#pragma once
#include "Scene\Torus.h"
#include "Core\Base.h"
#include "imgui.h"
#include "misc\cpp\imgui_stdlib.h"

namespace CADMageddon
{
    void TorusEditor(Ref<Torus> torus)
    {
        ImGui::BeginGroup();

        auto name = torus->GetName();
        if (ImGui::InputText("Name", &name))
        {
            torus->SetName(name);
        }

        auto position = torus->GetTransform()->Translation;
        if (ImGui::DragFloat3("Position", &position.x))
        {
            torus->GetTransform()->Translation = position;
        }

        auto rotation = torus->GetTransform()->Rotation;
        if (ImGui::DragFloat3("Rotation", &rotation.x))
        {
            torus->GetTransform()->Rotation = rotation;
        }

        auto scale = torus->GetTransform()->Scale;
        if (ImGui::DragFloat3("Scale", &scale.x))
        {
            torus->GetTransform()->Scale = scale;
        }

        auto& torusParameters = torus->GetTorusParameters();

        bool changed = false;

        auto majorRadius = torusParameters.MajorRadius;
        if (ImGui::DragFloat("MajorRadius", &majorRadius))
        {
            torusParameters.MajorRadius = majorRadius;
            changed = true;
        }

        auto minorRadius = torusParameters.MinorRadius;
        if (ImGui::DragFloat("MinorRadius", &minorRadius))
        {
            torusParameters.MinorRadius = minorRadius;
            changed = true;
        }

        auto majorRadiusCount = torusParameters.MajorRadiusCount;
        if (ImGui::DragInt("MajorRadiusCount", &majorRadiusCount))
        {
            torusParameters.MajorRadiusCount = majorRadiusCount;
            changed = true;
        }

        auto minorRadiusCount = torusParameters.MinorRadiusCount;
        if (ImGui::DragInt("MinorRadiusCount", &minorRadiusCount))
        {
            torusParameters.MinorRadiusCount = minorRadiusCount;
            changed = true;
        }

        if (changed)
        {
            torus->RecalculateMesh();
        }

        ImGui::EndGroup();
    }
}
