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
        if (ImGui::DragFloat3("Position", &position.x, 0.1f))
        {
            torus->GetTransform()->Translation = position;
        }

        auto rotation = torus->GetTransform()->Rotation;
        if (ImGui::DragFloat3("Rotation", &rotation.x))
        {
            torus->GetTransform()->Rotation = rotation;
        }

        auto scale = torus->GetTransform()->Scale;
        if (ImGui::DragFloat3("Scale", &scale.x, 0.1f))
        {
            torus->GetTransform()->Scale = scale;
        }

        auto& torusParameters = torus->GetTorusParameters();

        bool changed = false;

        auto majorRadius = torusParameters.MajorRadius;
        if (ImGui::DragFloat("MajorRadius", &majorRadius, 0.1f, 0.1f, 20.0f))
        {
            majorRadius = std::clamp(majorRadius, 0.1f, 20.0f);
            torusParameters.MajorRadius = majorRadius;
            changed = true;
        }

        auto minorRadius = torusParameters.MinorRadius;
        if (ImGui::DragFloat("MinorRadius", &minorRadius, 0.1f, 0.1f, 20.0f))
        {
            minorRadius = std::clamp(minorRadius, 0.1f, 20.0f);
            torusParameters.MinorRadius = minorRadius;
            changed = true;
        }

        auto majorRadiusCount = torusParameters.MajorRadiusCount;
        if (ImGui::DragInt("MajorRadiusCount", &majorRadiusCount), 1.0f, 3, 100)
        {
            majorRadiusCount = std::clamp(majorRadiusCount, 3, 100);
            torusParameters.MajorRadiusCount = majorRadiusCount;
            changed = true;
        }

        auto minorRadiusCount = torusParameters.MinorRadiusCount;
        if (ImGui::DragInt("MinorRadiusCount", &minorRadiusCount), 1.0f, 3, 100)
        {
            minorRadiusCount = std::clamp(minorRadiusCount, 3, 100);
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
