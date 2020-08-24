#pragma once
#include "Scene\BSpline.h"
#include "Core\Base.h"
#include "imgui.h"
#include "misc\cpp\imgui_stdlib.h"

namespace CADMageddon
{
    void BSplineEditor(Ref<BSpline> bSpline)
    {
        ImGui::BeginGroup();

        auto name = bSpline->GetName();
        if (ImGui::InputText("Name", &name))
        {
            bSpline->SetName(name);
        }

        bool showBSplinePolygon = bSpline->GetShowBSplinePolygon();
        if (ImGui::Checkbox("ShowBSplinePolygon", &showBSplinePolygon))
        {
            bSpline->SetShowBSplinePolygon(showBSplinePolygon);
        }

        bool isBezierBasis = bSpline->GetIsBezierBasis();
        if (ImGui::Checkbox("BezierBasis", &isBezierBasis))
        {
            bSpline->SetIsBezierBasis(isBezierBasis);
        }

        bool showBezierPolygon = bSpline->GetShowBezierPolygon();
        if (isBezierBasis && ImGui::Checkbox("ShowBezierPolygon", &showBezierPolygon))
        {
            bSpline->SetShowBezierPolygon(showBezierPolygon);
        }

        ImGui::EndGroup();
    }
}