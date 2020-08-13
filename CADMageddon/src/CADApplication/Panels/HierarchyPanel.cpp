#include "HierarchyPanel.h"
#include "Scene\Entity.h"
#include "Scene\Components.h"
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

        int id = 0;
        for (auto entity : m_Scene->GetEntities())
        {
            if (!entity.HasComponent<NameComponent>() || !entity.HasComponent<HierarchyComponent>())
            {
                continue;
            }

            RenderNode(entity, id);
        }

        ImGui::End();
    }

    void HierarchyPanel::HandleSingleSelection(Entity entity, HierarchyComponent& hierarchyComponent)
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
    }

    void HierarchyPanel::ClearSelection()
    {
        std::vector<Entity> cleared;

        for (auto entity : m_Scene->GetEntities())
        {
            if (entity.HasComponent<HierarchyComponent>())
            {
                auto& hierarchyComponent = entity.GetComponent<HierarchyComponent>();
                if (hierarchyComponent.IsSelected)
                {
                    cleared.push_back(entity);
                    hierarchyComponent.IsSelected = false;
                }
            }
        }

        if (m_OnSelectionCleared)
            m_OnSelectionCleared(cleared);
    }

    void HierarchyPanel::AddPointsToBezier(BezierC0Component& bezierComponent)
    {
        for (auto entity : m_Scene->GetEntities())
        {
            if (entity.HasComponent<PointComponent>()
                && entity.HasComponent<HierarchyComponent>()
                && entity.GetComponent<HierarchyComponent>().IsSelected)
            {
                Ref<Point> p = entity.GetComponent<PointComponent>().Point;
                bezierComponent.ControlPoints.push_back(p);
            }
        }
    }

    void HierarchyPanel::RenderNode(Entity entity, int& id)
    {
        static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

        auto& nameComponent = entity.GetComponent<NameComponent>();
        auto& hierarchyComponent = entity.GetComponent<HierarchyComponent>();

        ImGuiTreeNodeFlags node_flags = base_flags;

        if (hierarchyComponent.IsSelected)
        {
            node_flags |= ImGuiTreeNodeFlags_Selected;
        }

        //if (hierarchyComponent.Children.empty())
        {
            node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
        }

        bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)id, node_flags, nameComponent.Name.c_str());
        if (ImGui::BeginPopupContextItem())
        {
            if (entity.HasComponent<BezierC0Component>() && ImGui::MenuItem("Add Points"))
            {
                AddPointsToBezier(entity.GetComponent<BezierC0Component>());
            }

            ImGui::EndPopup();
        }


        if (ImGui::IsItemClicked())
        {
            if (ImGui::GetIO().KeyCtrl)
            {
                HandleMultiSelection(entity, hierarchyComponent);
            }
            else
            {
                HandleSingleSelection(entity, hierarchyComponent);
            }
        }

        //if (node_open)
        //{
        //    /*for (auto child : hierarchyComponent.Children)
        //    {
        //        RenderNode(child, id);
        //    }*/

        //    ImGui::TreePop();
        //}

        id++;
    }
}
