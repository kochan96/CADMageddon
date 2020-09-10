#pragma once
#include "Scene\SurfaceUV.h"
#include "imgui.h"

namespace CADMageddon
{
    void SurfaceEditor(Ref<SurfaceUV> surface)
    {
        ImGui::BeginGroup();

        bool isTrimmed = surface->GetIsTrimmed();
        ImGui::Text("Is trimmed: %s", surface->GetIsTrimmed() ? "Yes" : "No");

        if (isTrimmed)
        {
            bool reverseTrimming = surface->GetReverseTrimming();
            if (ImGui::Checkbox("Reverse Trimming", &reverseTrimming))
            {
                surface->SetReverseTrimming(reverseTrimming);
            }

            auto trimmingType = surface->GetTrimmingType();
            if (ImGui::RadioButton("TrimInside", trimmingType ==TrimmingType::Inside))
            {
                surface->SetTrimmingType(TrimmingType::Inside);
            }

            if (ImGui::RadioButton("TrimWithBoundary", trimmingType== TrimmingType::InsideWithBoundary))
            {
                surface->SetTrimmingType(TrimmingType::InsideWithBoundary);
            }
        }

        ImGui::EndGroup();
    }
}