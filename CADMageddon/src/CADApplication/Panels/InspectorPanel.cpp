#include "InspectorPanel.h"
#include "Scene\Entity.h"
#include "../ImguiEditors/ImguiEditors.h"

namespace CADMageddon
{
    void InspectorPanel::Add(Entity entity)
    {
        m_Entities.push_back(entity);
    }

    void InspectorPanel::Remove(Entity entity)
    {
        auto it = std::find(m_Entities.begin(), m_Entities.end(), entity);
        if (it != m_Entities.end())
        {
            m_Entities.erase(it);
        }
    }

    void InspectorPanel::Clear()
    {
        m_Entities.clear();
    }

    void InspectorPanel::Render()
    {
        ImGui::Begin("Inspector");

        if (m_Entities.size() == 1)
        {
            auto m_SelectedEntity = m_Entities[0];

            if (m_SelectedEntity.HasComponent<NameComponent>())
            {
                auto& nameComponent = m_SelectedEntity.GetComponent<NameComponent>();
                NameComponentEditor(nameComponent);
            }

            if (m_SelectedEntity.HasComponent<PointComponent>())
            {
                auto& pointComponent = m_SelectedEntity.GetComponent<PointComponent>();
                PointComponentEditor(pointComponent);
            }

            if (m_SelectedEntity.HasComponent<TransformComponent>())
            {
                auto& transformComponent = m_SelectedEntity.GetComponent<TransformComponent>();
                TransformComponentEditor(transformComponent);
            }

            if (m_SelectedEntity.HasComponent<TorusComponent>())
            {
                auto& torusComponent = m_SelectedEntity.GetComponent<TorusComponent>();
                TorusComponentEditor(torusComponent);
            }

            if (m_SelectedEntity.HasComponent<BezierC0Component>())
            {
                auto& bezierC0Component = m_SelectedEntity.GetComponent<BezierC0Component>();
                BezierC0ComponentEditor(bezierC0Component);
            }
        }

        ImGui::End();
    }
}

