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

    void InspectorPanel::Clear()
    {
        m_Points.clear();
        m_Torus.clear();
        m_Bezier.clear();
    }

    void InspectorPanel::Render()
    {
        ImGui::Begin("Inspector");

        unsigned int size = m_Points.size() + m_Torus.size() + m_Bezier.size();
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

        ImGui::End();
    }
}

