#include "HierarchyPanel.h"
#include "Scene\Scene.h"
#include "imgui.h"

namespace CADMageddon
{
    HierarchyPanel::HierarchyPanel(Ref<Scene> scene)
        :m_Scene(scene)
    {

    }

    void HierarchyPanel::Render()
    {
        ImGui::Begin("Hierarchy");

        auto points = m_Scene->GetFreePoints();
        if (!points.empty() && ImGui::TreeNode("Points"))
        {
            int id = 0;
            for (auto point : points)
            {
                RenderPointNode(point, id);
            }

            ImGui::TreePop();
        }

        auto toruses = m_Scene->GetTorus();
        if (!toruses.empty() && ImGui::TreeNode("Toruses"))
        {
            int id = 0;
            for (auto torus : toruses)
            {
                RenderTorusNode(torus, id);
            }

            ImGui::TreePop();
        }

        auto beziersC0 = m_Scene->GetBezierC0();
        if (!beziersC0.empty() && ImGui::TreeNode("BezierC0"))
        {
            int id = 0;
            for (auto bezierC0 : beziersC0)
            {
                RenderBezierC0Node(bezierC0, id);
            }

            ImGui::TreePop();
        }

        ImGui::End();
    }


    void HierarchyPanel::ClearSelection()
    {
        for (auto point : m_Scene->GetFreePoints())
        {
            point->SetIsSelected(false);
        }

        for (auto torus : m_Scene->GetTorus())
        {
            torus->SetIsSelected(false);
        }

        if (m_OnSelectionCleared)
            m_OnSelectionCleared();
    }


    void HierarchyPanel::RenderPointNode(Ref<Point> point, int& id)
    {
        static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
        ImGuiTreeNodeFlags node_flags = base_flags;

        if (point->GetIsSelected())
        {
            node_flags |= ImGuiTreeNodeFlags_Selected;
        }

        node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

        bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)id, node_flags, point->GetName().c_str());
        if (ImGui::IsItemClicked())
        {
            if (ImGui::GetIO().KeyCtrl)
            {
                point->SetIsSelected(!point->GetIsSelected());
                if (m_OnSelectionPointChanged)
                    m_OnSelectionPointChanged(point->GetIsSelected(), point);
            }
            else
            {
                bool oldSelected = point->GetIsSelected();
                ClearSelection();
                point->SetIsSelected(!oldSelected);
                if (m_OnSelectionPointChanged)
                    m_OnSelectionPointChanged(point->GetIsSelected(), point);
            }
        }

        id++;
    }

    void HierarchyPanel::RenderTorusNode(Ref<Torus> torus, int& id)
    {
        static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
        ImGuiTreeNodeFlags node_flags = base_flags;

        if (torus->GetIsSelected())
        {
            node_flags |= ImGuiTreeNodeFlags_Selected;
        }

        node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

        bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)id, node_flags, torus->GetName().c_str());
        if (ImGui::IsItemClicked())
        {
            if (ImGui::GetIO().KeyCtrl)
            {
                torus->SetIsSelected(!torus->GetIsSelected());
                if (m_OnSelectionTorusChanged)
                    m_OnSelectionTorusChanged(torus->GetIsSelected(), torus);
            }
            else
            {
                bool oldSelected = torus->GetIsSelected();
                ClearSelection();
                torus->SetIsSelected(!oldSelected);
                if (m_OnSelectionTorusChanged)
                    m_OnSelectionTorusChanged(torus->GetIsSelected(), torus);
            }
        }

        id++;
    }

    void HierarchyPanel::RenderBezierC0Node(Ref<BezierC0> bezierC0, int& id)
    {
        static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
        ImGuiTreeNodeFlags node_flags = base_flags;

        if (bezierC0->GetIsSelected())
        {
            node_flags |= ImGuiTreeNodeFlags_Selected;
        }

        bool isBezierEmpty = bezierC0->GetControlPoints().empty();

        if (isBezierEmpty)
        {
            node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
        }

        bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)id, node_flags, bezierC0->GetName().c_str());
        if (ImGui::BeginPopupContextItem())
        {
            if (ImGui::MenuItem("AssignPoints"))
            {
                m_Scene->AssignSelectedFreeToBezier(bezierC0);
            }

            ImGui::EndPopup();
        }

        if (ImGui::IsItemClicked())
        {
            if (ImGui::GetIO().KeyCtrl)
            {
                bezierC0->SetIsSelected(!bezierC0->GetIsSelected());
            }
            else
            {
                bool oldSelected = bezierC0->GetIsSelected();
                ClearSelection();
                bezierC0->SetIsSelected(!oldSelected);
            }
        }

        if (node_open && !isBezierEmpty)
        {
            auto points = bezierC0->GetControlPoints();
            for (auto point : points)
            {
                RenderBezierControlPointNode(bezierC0, point, id);
            }

            ImGui::TreePop();
        }

        id++;
    }

    void HierarchyPanel::RenderBezierControlPointNode(Ref<BezierC0> bezierc0, Ref<Point> point, int& id)
    {
        static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
        ImGuiTreeNodeFlags node_flags = base_flags;

        if (point->GetIsSelected())
        {
            node_flags |= ImGuiTreeNodeFlags_Selected;
        }

        node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

        bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)id, node_flags, point->GetName().c_str());
        if (ImGui::BeginPopupContextItem())
        {
            if (ImGui::MenuItem("Remove"))
            {
                m_Scene->RemovePointFromBezier(bezierc0, point);
            }

            ImGui::EndPopup();
        }

        if (ImGui::IsItemClicked())
        {
            if (ImGui::GetIO().KeyCtrl)
            {
                point->SetIsSelected(!point->GetIsSelected());
                if (m_OnSelectionPointChanged)
                    m_OnSelectionPointChanged(point->GetIsSelected(), point);
            }
            else
            {
                bool oldSelected = point->GetIsSelected();
                ClearSelection();
                point->SetIsSelected(!oldSelected);
                if (m_OnSelectionPointChanged)
                    m_OnSelectionPointChanged(point->GetIsSelected(), point);
            }
        }

        id++;
    }
}
