#include "InspectorPanel.h"
#include "CADApplication\ImGuiEditors\ImGuiEditors.h"

namespace CADMageddon
{
    void InspectorPanel::AddPoint(Ref<Point> point)
    {
        m_Points.push_back(point);
    }

    void InspectorPanel::RemovePoint(Ref<Point> point)
    {
        auto it = std::find(m_Points.begin(), m_Points.end(), point);
        if (it != m_Points.end())
        {
            m_Points.erase(it);
        }
    }

    void InspectorPanel::AddTorus(Ref<Torus> torus)
    {
        m_Torus.push_back(torus);

    }

    void InspectorPanel::RemoveTorus(Ref<Torus> torus)
    {
        auto it = std::find(m_Torus.begin(), m_Torus.end(), torus);
        if (it != m_Torus.end())
        {
            m_Torus.erase(it);
        }
    }

    void InspectorPanel::AddBezierC0(Ref<BezierC0> bezier)
    {
        m_Bezier.push_back(bezier);
    }

    void InspectorPanel::RemoveBezierC0(Ref<BezierC0> bezier)
    {
        auto it = std::find(m_Bezier.begin(), m_Bezier.end(), bezier);
        if (it != m_Bezier.end())
            m_Bezier.erase(it);
    }

    void InspectorPanel::AddBSpline(Ref<BSpline> bSpline)
    {
        m_BSpline.push_back(bSpline);
    }

    void InspectorPanel::RemoveBSpline(Ref<BSpline> bSpline)
    {
        auto it = std::find(m_BSpline.begin(), m_BSpline.end(), bSpline);
        if (it != m_BSpline.end())
            m_BSpline.erase(it);
    }

    void InspectorPanel::AddInterpolatedCurve(Ref<InterpolatedCurve> interPolatedCurve)
    {
        m_InterPolatedCurve.push_back(interPolatedCurve);
    }

    void InspectorPanel::RemoveInterpolatedCurve(Ref<InterpolatedCurve> interPolatedCurve)
    {
        auto it = std::find(m_InterPolatedCurve.begin(), m_InterPolatedCurve.end(), interPolatedCurve);
        if (it != m_InterPolatedCurve.end())
            m_InterPolatedCurve.erase(it);
    }

    void InspectorPanel::AddBezierPatch(Ref<BezierPatch> bezierPatch)
    {
        m_BezierPatch.push_back(bezierPatch);
    }

    void InspectorPanel::RemoveBezierPatch(Ref<BezierPatch> bezierPatch)
    {
        auto it = std::find(m_BezierPatch.begin(), m_BezierPatch.end(), bezierPatch);
        if (it != m_BezierPatch.end())
            m_BezierPatch.erase(it);
    }

    void InspectorPanel::AddBSplinePatch(Ref<BSplinePatch> bSplinePatch)
    {
        m_BSplinePatch.push_back(bSplinePatch);
    }

    void InspectorPanel::RemoveBSplinePatch(Ref<BSplinePatch> bSplinePatch)
    {
        auto it = std::find(m_BSplinePatch.begin(), m_BSplinePatch.end(), bSplinePatch);
        if (it != m_BSplinePatch.end())
            m_BSplinePatch.erase(it);
    }

    void InspectorPanel::AddGregoryPatch(Ref<GregoryPatch> gregoryPatch)
    {
        m_GregoryPatch.push_back(gregoryPatch);
    }

    void InspectorPanel::RemoveGregoryPatch(Ref<GregoryPatch> gregoryPatch)
    {
        auto it = std::find(m_GregoryPatch.begin(), m_GregoryPatch.end(), gregoryPatch);
        if (it != m_GregoryPatch.end())
            m_GregoryPatch.erase(it);
    }

    void InspectorPanel::ClearPointsAndToruses()
    {
        m_Points.clear();
        m_Torus.clear();
    }

    void InspectorPanel::Clear()
    {
        m_Points.clear();
        m_Torus.clear();
        m_Bezier.clear();
        m_BSpline.clear();
        m_InterPolatedCurve.clear();
        m_BezierPatch.clear();
        m_BSplinePatch.clear();
        m_GregoryPatch.clear();
    }

    void InspectorPanel::RenderMultiSelectInspector()
    {
        if (m_transformationSystem->GetCount() > 1)
        {
            ImGui::BeginGroup();
            ImGui::Text("Transform");
            m_transformationSystem->RenderImGui();
            ImGui::EndGroup();
        }

        if (m_Points.size() == 2)
        {
            ImGui::BeginGroup();
            if (ImGui::Button("MergePoints"))
            {
                m_Scene->MergePoints(m_Points[0], m_Points[1]);
            }
            ImGui::EndGroup();
        }

        if (m_BezierPatch.size() == 3)
        {
            RenderFillHoleInspector();
        }
    }

    void InspectorPanel::RenderFillHoleInspector()
    {
        auto b1 = m_BezierPatch[0];
        auto b2 = m_BezierPatch[1];
        auto b3 = m_BezierPatch[2];

        bool areAllSinglePatch = b1->GetPatchCountX() == 1 && b1->GetPatchCountY() == 1
            && b2->GetPatchCountX() == 1 && b2->GetPatchCountY() == 1
            && b3->GetPatchCountX() == 1 && b3->GetPatchCountY() == 1;

        if (!areAllSinglePatch)
            return;

        Ref<Point> commonPoints[3];
        if (!GetCommonPoint(b1, b2, commonPoints[0]))
            return;
        if (!GetCommonPoint(b2, b3, commonPoints[1]))
            return;
        if (!GetCommonPoint(b3, b1, commonPoints[2]))
            return;

        if (!CheckIfCorner(b1, commonPoints[0]))
            return;
        if (!CheckIfCorner(b2, commonPoints[0]))
            return;
        if (!CheckIfCorner(b2, commonPoints[1]))
            return;
        if (!CheckIfCorner(b3, commonPoints[1]))
            return;
        if (!CheckIfCorner(b3, commonPoints[2]))
            return;
        if (!CheckIfCorner(b1, commonPoints[2]))
            return;

        if (ImGui::Button("Fill hole"))
        {
            m_Scene->CreateGregoryPatch(m_BezierPatch[0], m_BezierPatch[1], m_BezierPatch[2], commonPoints);
        }
    }

    bool InspectorPanel::GetCommonPoint(Ref<BezierPatch> b1, Ref<BezierPatch> b2, Ref<Point>& commonPoint)
    {
        auto b1ControlPoints = b1->GetControlPoints();
        auto b2ControloPoints = b2->GetControlPoints();

        std::vector<Ref<Point>> commonPoints;
        for (int i = 0; i < b1ControlPoints.size(); i++)
        {
            for (int j = 0; j < b2ControloPoints.size(); j++)
            {
                if (b1ControlPoints[i] == b2ControloPoints[j])
                {
                    commonPoints.push_back(b1ControlPoints[i]);
                    if (commonPoints.size() > 1)
                    {
                        return false;
                    }
                }
            }
        }

        if (commonPoints.empty())
            return false;

        commonPoint = commonPoints[0];

        return true;
    }

    bool InspectorPanel::CheckIfCorner(Ref<BezierPatch> b, Ref<Point> commonPoint)
    {
        const int cornersSize = 4;
        int corners[] = { 0,3,12,15 };
        auto controlPoints = b->GetControlPoints();

        for (int i = 0; i < cornersSize; i++)
        {
            if (commonPoint == controlPoints[corners[i]])
                return true;
        }

        return false;
    }

    void InspectorPanel::Render()
    {
        ImGui::Begin("Inspector");

        unsigned int size =
            m_Points.size() +
            m_Torus.size() +
            m_Bezier.size() +
            m_BSpline.size() +
            m_InterPolatedCurve.size() +
            m_BezierPatch.size() +
            m_BSplinePatch.size()
            + m_GregoryPatch.size();

        ImGui::Text("Selected %d items", size);

        if (size == 0)
        {
            ImGui::End();
            return;
        }

        if (size > 1)
        {
            RenderMultiSelectInspector();
            ImGui::End();
            return;
        }

        if (m_Points.size() == 1)
        {
            PointEditor(m_Points[0]);
        }
        else if (m_Torus.size() == 1)
        {
            TorusEditor(m_Torus[0]);
        }
        else if (m_Bezier.size() == 1)
        {
            BezierEditor(m_Bezier[0]);
        }
        else if (m_BSpline.size() == 1)
        {
            BSplineEditor(m_BSpline[0]);
        }
        else if (m_InterPolatedCurve.size() == 1)
        {
            InterpolatedCurveEditor(m_InterPolatedCurve[0]);
        }
        else if (m_BezierPatch.size() == 1)
        {
            BezierPatchEditor(m_BezierPatch[0]);
        }
        else if (m_BSplinePatch.size() == 1)
        {
            BSplinePatchEditor(m_BSplinePatch[0]);
        }
        else if (m_GregoryPatch.size() == 1)
        {
            GregoryEditor(m_GregoryPatch[0]);
        }


        ImGui::End();
    }
}

