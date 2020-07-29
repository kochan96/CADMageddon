#pragma once
#include <imgui.h>
#include "Scene\Components.h"

namespace CADMageddon
{
    bool TorusEditor(TorusComponent& component)
    {
        ImGui::BeginChild("Torus");

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
            if (count > 2)
            {
                component.MajorRadiusCount = count;
                changed = true;
            }
        }

        count = component.MinorRadiusCount;
        if (ImGui::DragInt("MinorRadiusCount", &count))
        {
            if (count > 2)
            {
                component.MinorRadiusCount = count;
                changed = true;
            }
        }

        if (changed)
        {
            component.Mesh = ObjectFactory::CreateTorusMesh(component.MajorRadius, component.MinorRadius, component.MajorRadiusCount, component.MinorRadiusCount);
        }


        ImGui::EndChild();

        return true;
    }
}