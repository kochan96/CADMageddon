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
            m_BSplinePatch.size();

        if (size != 1)
        {
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
        else if (m_BSplinePatch.size())
        {
            BSplinePatchEditor(m_BSplinePatch[0]);
        }

        ImGui::End();
    }
}

