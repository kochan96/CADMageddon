#pragma once
#include "Scene\Scene.h"
#include "Rendering\Camera.h"
#include "Scene\Components.h"
#include <glm\glm.hpp>
#include "cadpch.h"

namespace CADMageddon
{
    enum class TransformationMode
    {
        Translation,
        Rotation,
        Scaling
    };

    enum class TransformationOrigin
    {
        Center,
        Cursor,
    };

    class Cursor3D;

    class TransformationSystem
    {
    public:
        TransformationSystem(Ref<Cursor3D> cursor);
        void Update(Ref<Scene> scene, FPSCamera& camera, glm::vec2 ndcMousePosition);

        void AddToSelected(Entity entity);
        void RemoveFromSelected(Entity entity);
        void ClearSelection();

        TransformationMode GetTransformationMode() const { return m_TransformationMode; }
        void SetTransformationMode(TransformationMode mode) { m_TransformationMode = mode; RecalculateParentAndChildrenTransform(); }

        TransformationOrigin GetTransformationOrigin() const { return m_TransformationOrigin; }
        void SetTransformationOrigin(TransformationOrigin origin) { m_TransformationOrigin = origin; RecalculateParentAndChildrenTransform(); }

    private:
        TransformComponent& GetTransformToModify();
        void AssignParentTransform(TransformComponent& transform);
        void UnAssignParentTransform(TransformComponent& transform);
        void RecalculateParentAndChildrenTransform();

        glm::vec3 GetTransformCenter();

    private:
        TransformationMode m_TransformationMode;
        TransformationOrigin m_TransformationOrigin;
        std::vector<Entity> m_SelectedEntities;
        
        Ref<Cursor3D> m_Cursor;
        Ref<TransformComponent> m_TransformationParent;
    };
}