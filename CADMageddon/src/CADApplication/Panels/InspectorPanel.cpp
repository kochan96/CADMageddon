#include "InspectorPanel.h"
#include "CADApplication\ImGuiEditors\ImGuiEditors.h"
#include "Scene/IntersectionHelper.h"
#include "misc\cpp\imgui_stdlib.h"
#include "ImGui\implot.h"

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
        m_Surfaces.push_back(torus);
    }

    void InspectorPanel::RemoveTorus(Ref<Torus> torus)
    {
        auto it = std::find(m_Torus.begin(), m_Torus.end(), torus);
        if (it != m_Torus.end())
        {
            m_Torus.erase(it);
            auto it2 = std::find(m_Surfaces.begin(), m_Surfaces.end(), torus);
            m_Surfaces.erase(it2);
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
        m_Surfaces.push_back(bezierPatch);
    }

    void InspectorPanel::RemoveBezierPatch(Ref<BezierPatch> bezierPatch)
    {
        auto it = std::find(m_BezierPatch.begin(), m_BezierPatch.end(), bezierPatch);
        if (it != m_BezierPatch.end())
        {
            m_BezierPatch.erase(it);
            auto it2 = std::find(m_Surfaces.begin(), m_Surfaces.end(), bezierPatch);
            m_Surfaces.erase(it2);
        }
    }

    void InspectorPanel::AddBSplinePatch(Ref<BSplinePatch> bSplinePatch)
    {
        m_BSplinePatch.push_back(bSplinePatch);
        m_Surfaces.push_back(bSplinePatch);
    }

    void InspectorPanel::RemoveBSplinePatch(Ref<BSplinePatch> bSplinePatch)
    {
        auto it = std::find(m_BSplinePatch.begin(), m_BSplinePatch.end(), bSplinePatch);
        if (it != m_BSplinePatch.end())
        {
            m_BSplinePatch.erase(it);
            auto it2 = std::find(m_Surfaces.begin(), m_Surfaces.end(), bSplinePatch);
            m_Surfaces.erase(it2);
        }
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

    void InspectorPanel::AddIntersectionCurve(Ref<IntersectionCurve> intersectionCurve)
    {
        m_IntersectionCurves.push_back(intersectionCurve);
    }

    void InspectorPanel::RemoveIntersectionCurve(Ref<IntersectionCurve> intersectionCurve)
    {
        auto it = std::find(m_IntersectionCurves.begin(), m_IntersectionCurves.end(), intersectionCurve);
        if (it != m_IntersectionCurves.end())
            m_IntersectionCurves.erase(it);
    }

    void InspectorPanel::ClearPointsAndToruses()
    {
        m_Points.clear();

        for (auto torus : m_Torus)
        {
            auto it = std::find(m_Surfaces.begin(), m_Surfaces.end(), torus);
            m_Surfaces.erase(it);
        }

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
        m_Surfaces.clear();
        m_IntersectionCurves.clear();
    }

    void InspectorPanel::RenderIntersectionPlot(Ref<IntersectionCurve> intersectionCurve)
    {
        bool showPlot = intersectionCurve->GetShowPlot();
        if (!showPlot)
        {
            return;
        }

        auto intersectionPoints = intersectionCurve->GetIntersectionPoints();

        ImGui::Begin("Intersection Plot");
        ImPlot::SetNextPlotLimits(0, 1, 0, 1);
        std::vector<float> x(intersectionPoints.size());
        std::vector<float> y(intersectionPoints.size());
        float offset[] = { 1,0,-1 };
        ImVec4 colors[] = { ImVec4(1,1,1,1),ImVec4(1,1,1,1) };
        ImPlot::SetColormap(&colors[0], 2);
        if (ImPlot::BeginPlot("plot 1", "U", "V", ImVec2(-1, 0), ImPlotFlags_Default ^ ImPlotFlags_Legend))
        {
            for (int j = 0; j < 3; j++)
            {
                for (int k = 0; k < 3; ++k)
                {
                    for (int i = 0; i < intersectionPoints.size(); i++)
                    {
                        x[i] = intersectionPoints[i].Coords.s + offset[j];
                        y[i] = intersectionPoints[i].Coords.t + offset[k];
                    }

                    ImPlot::PlotLine("#polygon1",
                        x.data(), y.data(), x.size(), 0);
                }
            }


            auto pixels = intersectionCurve->GetFirstSurfaceIntersectionLines();
            for (int i = 1; i < pixels.size(); i += 2)
            {
                ImVec2 points[2] = {
                    ImVec2(pixels[i - 1].x,pixels[i - 1].y),
                    ImVec2(pixels[i].x,pixels[i].y),
                };

                ImGui::PushID(i);
                ImPlot::PlotLine("#scatter1", points, 2, 0);
                ImGui::PopID();
            }


            ImPlot::EndPlot();
        }

        if (ImPlot::BeginPlot("plot 2", "U", "V", ImVec2(-1, 0), ImPlotFlags_Default ^ ImPlotFlags_Legend))
        {
            for (int j = 0; j < 3; j++)
            {
                for (int k = 0; k < 3; ++k)
                {
                    for (int i = 0; i < intersectionPoints.size(); i++)
                    {
                        x[i] = intersectionPoints[i].Coords.p + offset[j];
                        y[i] = intersectionPoints[i].Coords.q + offset[k];
                    }

                    ImPlot::PlotLine("#polygon2",
                        x.data(), y.data(), x.size(), 0);
                }
            }


            auto pixels = intersectionCurve->GetSecondSurfaceIntersectionLines();
            for (int i = 1; i < pixels.size(); i += 2)
            {
                ImVec2 points[2] = {
                    ImVec2(pixels[i - 1].x,pixels[i - 1].y),
                    ImVec2(pixels[i].x,pixels[i].y),
                };

                ImGui::PushID(i);
                ImPlot::PlotLine("#scatter1", points, 2, 0);
                ImGui::PopID();
            }

            /*if (curve->s1->isTrimmed)
                RenderPlotGrid(curve, 0);*/
            ImPlot::EndPlot();
        }


        if (ImGui::Button("Close window"))
        {
            intersectionCurve->SetShowPlot(false);
        }

        ImGui::End();

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

    void InspectorPanel::RenderFindIntersectionInspector()
    {
        Ref<SurfaceUV> s1;
        Ref<SurfaceUV> s2;
        GetIntersectionSurfaces(s1, s2);
        if (!s1 || !s2)
            return;

        ImGui::BeginGroup();

        ImGui::Text("Find Intersection");
        ImGui::Checkbox("BeginFromCursor", &m_BeginFromCursor);
        ImGui::DragFloat("Step length", &m_StepLength, 0.001f, 0.001f, 1.0f);
        if (ImGui::Button("Calculate Intersection"))
        {
            IntersectionType intersectionType = IntersectionType::OpenOpen;
            auto points = IntersectionHelper::GetIntersectionPoints(s1, s2, m_StepLength, intersectionType);
            if (points.empty())
            {
                m_IntersectionNotFound = true;
            }
            else
            {
                m_IntersectionNotFound = false;
                m_Scene->CreateIntersectionCurve("Intersected", s1, s2, points, intersectionType);
            }
        }


        ImGui::EndGroup();
    }

    void InspectorPanel::GetIntersectionSurfaces(Ref<SurfaceUV>& s1, Ref<SurfaceUV>& s2)
    {
        if (m_Surfaces.size() == 1)
        {
            s1 = m_Surfaces[0];
            s2 = m_Surfaces[0];
        }
        else if (m_Surfaces.size() == 2)
        {
            s1 = m_Surfaces[0];
            s2 = m_Surfaces[1];
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
        if (m_IntersectionNotFound)
        {
            ImGui::OpenPopup("Intersection not found");
        }

        if (ImGui::BeginPopupModal("Intersection not found", &m_IntersectionNotFound))
        {
            ImGui::Text("Intersection has not been found");
            if (ImGui::Button("Ok"))
            {
                m_IntersectionNotFound = false;
            }
            ImGui::EndPopup();
        }

        ImGui::Begin("Inspector");

        unsigned int size =
            m_Points.size() +
            m_Torus.size() +
            m_Bezier.size() +
            m_BSpline.size() +
            m_InterPolatedCurve.size() +
            m_BezierPatch.size() +
            m_BSplinePatch.size()
            + m_GregoryPatch.size()
            + m_IntersectionCurves.size();

        ImGui::Text("Selected %d items", size);

        if (size == 0)
        {
            ImGui::End();
            return;
        }

        if (size > 1)
        {
            RenderMultiSelectInspector();
            RenderFindIntersectionInspector();
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
            RenderFindIntersectionInspector();
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
            RenderFindIntersectionInspector();
        }
        else if (m_BSplinePatch.size() == 1)
        {
            BSplinePatchEditor(m_BSplinePatch[0]);
            RenderFindIntersectionInspector();
        }
        else if (m_GregoryPatch.size() == 1)
        {
            GregoryEditor(m_GregoryPatch[0]);
        }
        else if (m_IntersectionCurves.size() == 1)
        {
            auto name = m_IntersectionCurves[0]->GetName();
            if (ImGui::InputText("Name", &name))
            {
                m_IntersectionCurves[0]->SetName(name);
            }

            ImGui::BeginGroup();
            if (ImGui::Button("Convert to interpolated"))
            {
                static int count = 0;
                auto interpolated = m_IntersectionCurves[0]->ConvertToInterpolated("IntersectionInterpolated_" + std::to_string(count++));
                m_Scene->CreateInterpolated(interpolated);
            }

            if (ImGui::Button("Show Plot"))
            {
                m_IntersectionCurves[0]->SetShowPlot(true);
            }

            auto intersectionType = m_IntersectionCurves[0]->GetIntersectionType();
            std::string intersectionTypeText = "ClosedClosed";
            switch (intersectionType)
            {
                case IntersectionType::ClosedClosed:
                    intersectionTypeText = "ClosedClosed";
                    break;
                case IntersectionType::ClosedOpen:
                    intersectionTypeText = "ClosedOpen";
                    break;
                case IntersectionType::OpenClosed:
                    intersectionTypeText = "OpenClosed";
                    break;
                case IntersectionType::OpenOpen:
                    intersectionTypeText = "OpenOpen";
            }


            ImGui::Text("IsClosed: %s", intersectionTypeText);

            RenderIntersectionPlot(m_IntersectionCurves[0]);


            ImGui::EndGroup();
        }

        ImGui::End();
    }
}

