#pragma once
#include "Scene/BaseObject.h"
#include "imgui.h"

namespace CADMageddon
{
    void BaseObjectEditor(Ref<BaseObject> baseObject)
    {
        ImGui::BeginGroup();

        auto isVisible = baseObject->GetIsVisible();
        if (ImGui::Checkbox("IsVisble", &isVisible))
        {
            baseObject->SetIsVisible(isVisible);
        }

        ImGui::EndGroup();
    }
}