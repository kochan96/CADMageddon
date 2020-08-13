#pragma once
#include "cadpch.h"
#include "Scene\Components.h"
#include "imgui.h"
#include "misc\cpp\imgui_stdlib.h"

namespace CADMageddon
{
    void BezierC0ComponentEditor(BezierC0Component& component)
    {
        ImGui::BeginGroup();

        ImGui::Text("BezierC0");

        ImGui::Checkbox("ShowPolygon", &component.ShowPolygon);

        ImGui::Separator();

        std::vector<Ref<Point>> toBeDeleted;

        for (int i = 0; i < component.ControlPoints.size(); i++)
        {
            auto point = component.ControlPoints[i];
            ImGui::PushID(i);
            std::string name = point->GetName();
            if (ImGui::InputText("Name", &name))
            {
                point->SetName(name);
            }

            ImGui::PopID();

           /* ImGui::PushID(i);
            if (ImGui::Button("Select"))
            {

            }
            ImGui::PopID();*/

            ImGui::SameLine();

            ImGui::PushID(i);
            if (ImGui::Button("Remove"))
            {
                toBeDeleted.push_back(point);
            }
            ImGui::PopID();
        }

        for (auto point : toBeDeleted)
        {
            auto it = std::find(component.ControlPoints.begin(), component.ControlPoints.end(), point);
            point->SetHierarchyVisible(true);
            component.ControlPoints.erase(it);
        }

        ImGui::EndGroup();
    }
}