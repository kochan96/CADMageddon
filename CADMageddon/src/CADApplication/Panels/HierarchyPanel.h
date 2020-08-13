#pragma once
#include "cadpch.h"
#include "Core/Base.h"

namespace CADMageddon
{
    class Entity;
    class Scene;
    struct HierarchyComponent;
    struct NameComponent;
    struct BezierC0Component;

    class HierarchyPanel
    {
    public:
        HierarchyPanel(Ref<Scene> scene);
        void Render();

        void SetOnSelectionChangedCallback(std::function<void(bool, Entity)> selectionChangedCallback)
        {
            m_OnSelectionChanged = selectionChangedCallback;
        }

        void SetOnSelectionClearedCallback(std::function<void(std::vector<Entity>)> selectionClearedCallback)
        {
            m_OnSelectionCleared = selectionClearedCallback;
        }

        void SetScene(Ref<Scene> scene) { m_Scene = scene; }

    private:
        void HandleSingleSelection(Entity entity, HierarchyComponent& component);
        void HandleMultiSelection(Entity entity, HierarchyComponent& component);
        void ClearSelection();

        void AddPointsToBezier(BezierC0Component& bezierComponent);
        void RenderNode(Entity entity, int& id);

    private:
        std::function<void(bool, Entity)> m_OnSelectionChanged;
        std::function<void(std::vector<Entity>)> m_OnSelectionCleared;
        Ref<Scene> m_Scene;

    };
}