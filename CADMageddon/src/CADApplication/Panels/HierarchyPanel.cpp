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

    /* void HierarchyPanel::HandleSingleSelection(Entity entity, HierarchyComponent& hierarchyComponent)
     {
         bool isSelectedPreviousValue = hierarchyComponent.IsSelected;
         ClearSelection();

         hierarchyComponent.IsSelected = !isSelectedPreviousValue;

         if (m_OnSelectionChanged)
             m_OnSelectionChanged(hierarchyComponent.IsSelected, entity);
     }

     void HierarchyPanel::HandleMultiSelection(Entity entity, HierarchyComponent& hierarchyComponent)
     {
         hierarchyComponent.IsSelected = !hierarchyComponent.IsSelected;

         if (m_OnSelectionChanged)
             m_OnSelectionChanged(hierarchyComponent.IsSelected, entity);
     }*/

    void HierarchyPanel::ClearSelection()
    {

    }


    void HierarchyPanel::RenderPointNode(Ref<Point> point, int& id)
    {
        static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
        ImGuiTreeNodeFlags node_flags = base_flags;

        if (point->GetIsSelected())
        {
            node_flags |= ImGuiTreeNodeFlags_Selected;
        }

        //if (hierarchyComponent.Children.empty())
        {
            node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
        }

        bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)id, node_flags, point->GetName().c_str());
        if (ImGui::IsItemClicked())
        {
            point->SetIsSelected(!point->GetIsSelected());
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

        //if (hierarchyComponent.Children.empty())
        {
            node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
        }

        bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)id, node_flags, torus->GetName().c_str());
        if (ImGui::IsItemClicked())
        {
            torus->SetIsSelected(!torus->GetIsSelected());
        }
    }
}
