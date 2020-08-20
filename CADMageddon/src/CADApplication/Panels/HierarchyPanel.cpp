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
    }
}
