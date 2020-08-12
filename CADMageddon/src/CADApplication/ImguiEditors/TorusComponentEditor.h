#pragma once
#include <imgui.h>
#include "Scene\Components.h"
#include "misc\cpp\imgui_stdlib.h"

namespace CADMageddon
{
    bool TorusComponentEditor(TorusComponent& component)
    {
        const int maxMajorCount = 200;
        const int maxMinorCount = 100;

        ImGui::BeginGroup();
        ImGui::Text("TorusComponent");

        bool changed = false;
        float value = component.MajorRadius;

        if (ImGui::DragFloat("MajorRadius", &value, 0.1f))
        {
            component.MajorRadius = value;
            changed = true;
        }

        value = component.MinorRadius;
        if (ImGui::DragFloat("MinorRadius", &value, 0.1f))
        {
            component.MinorRadius = value;
            changed = true;
        }

        int count = component.MajorRadiusCount;
        if (ImGui::DragInt("MajorRadiusCount", &count))
        {
            if (count > 2 && count <= maxMajorCount)
            {
                component.MajorRadiusCount = count;
                changed = true;
            }
        }

        count = component.MinorRadiusCount;
        if (ImGui::DragInt("MinorRadiusCount", &count))
        {
            if (count > 2 && count <= maxMinorCount)
            {
                component.MinorRadiusCount = count;
                changed = true;
            }
        }

        if (changed)
        {
            Mesh mesh = ObjectFactory::CreateTorusMesh(
                component.MajorRadius,
                component.MinorRadius,
                component.MajorRadiusCount,
                component.MinorRadiusCount);

            component.Points.clear();
            component.Points.insert(component.Points.begin(), mesh.Vertices.begin(), mesh.Vertices.end());
            component.Indices = mesh.Indices;
        }


        ImGui::Separator();

        if (ImGui::TreeNode("TorusPoints"))
        {
            int id = 0;

            for (auto point : component.Points)
            {
                auto name = point.GetName();
                ImGui::PushID(id++);
                if (ImGui::InputText("Name", &name))
                {
                    point.SetName(name);
                }

                ImGui::PopID();
            }

            ImGui::TreePop();
        }


        ImGui::EndGroup();

        return true;
    }
}