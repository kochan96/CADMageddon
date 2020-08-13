#include "TransformationSystem.h"
#include "Scene\Entity.h"
#include <glm\gtc\quaternion.hpp>
#include <glm\glm.hpp>
#include <glm\gtx\matrix_decompose.hpp>
#include "Gizmos\Gizmo.h"
#include "Cursor3D.h"

namespace CADMageddon
{

    ///TODO: fix rotation issues

    TransformationSystem::TransformationSystem(Ref<Cursor3D> cursor)
        :m_Cursor(cursor)
    {
        m_TransformationParent = CreateRef<TransformComponent>();
        m_TransformationMode = TransformationMode::Translation;
        m_TransformationOrigin = TransformationOrigin::Cursor;
    }

    void TransformationSystem::Update(Ref<Scene> scene, FPSCamera& camera, glm::vec2 ndcMousePosition)
    {
        if (m_SelectedEntities.empty())
        {
            return;
        }

        auto& transformToManipulate = GetTransformToModify();

        switch (m_TransformationMode)
        {
            case TransformationMode::Translation:
                Gizmo::Manipulate(GizmoMode::Translation, *transformToManipulate.Transform, camera.GetViewProjectionMatrix(), ndcMousePosition);
                break;
            case TransformationMode::Rotation:
                Gizmo::Manipulate(GizmoMode::Rotation, *transformToManipulate.Transform, camera.GetViewProjectionMatrix(), ndcMousePosition);
                break;
            case TransformationMode::Scaling:
                Gizmo::Manipulate(GizmoMode::Scale, *transformToManipulate.Transform, camera.GetViewProjectionMatrix(), ndcMousePosition);
                break;
        }

        if (m_TransformationOrigin == TransformationOrigin::Cursor && m_TransformationMode == TransformationMode::Translation)
        {
            RecalculateParentAndChildrenTransform();
        }
    }

    void TransformationSystem::AddToSelected(Entity entity)
    {
        if (entity.HasComponent<TransformComponent>())
        {
            m_SelectedEntities.push_back(entity);
            RecalculateParentAndChildrenTransform();
        }
    }

    void TransformationSystem::RemoveFromSelected(Entity entity)
    {
        auto it = std::find(m_SelectedEntities.begin(), m_SelectedEntities.end(), entity);
        if (it != m_SelectedEntities.end())
        {
            UnAssignParentTransform(it->GetComponent<TransformComponent>());
            m_SelectedEntities.erase(it);
            RecalculateParentAndChildrenTransform();
        }
    }

    void TransformationSystem::ClearSelection()
    {
        for (auto selected : m_SelectedEntities)
        {
            UnAssignParentTransform(selected.GetComponent<TransformComponent>());
        }

        m_SelectedEntities.clear();
    }

    TransformComponent& TransformationSystem::GetTransformToModify()
    {

        if (m_SelectedEntities.size() == 1 && m_TransformationOrigin == TransformationOrigin::Center)
        {
            return m_SelectedEntities[0].GetComponent<TransformComponent>();
        }
        else
        {
            return *m_TransformationParent;
        }
    }

    void TransformationSystem::AssignParentTransform(TransformComponent& transform)
    {
        transform.Transform->SetParent(m_TransformationParent->Transform);
    }

    void TransformationSystem::UnAssignParentTransform(TransformComponent& transform)
    {
        transform.Transform->SetParent(nullptr);
    }

    void TransformationSystem::RecalculateParentAndChildrenTransform()
    {
        if (m_SelectedEntities.empty())
        {
            return;
        }

        for (auto selected : m_SelectedEntities)
        {
            auto& transformComponent = selected.GetComponent<TransformComponent>();
            UnAssignParentTransform(transformComponent);
        }

        if (m_TransformationOrigin == TransformationOrigin::Center && m_SelectedEntities.size() == 1)
        {
            return;
        }

        auto center = GetTransformCenter();
        m_TransformationParent->Transform->Translation = center;
        m_TransformationParent->Transform->Rotation = glm::vec3(0.0f, 0.0f, 0.0f);
        m_TransformationParent->Transform->Scale = glm::vec3(1.0f, 1.0f, 1.0f);

        for (auto selected : m_SelectedEntities)
        {
            auto& transformComponent = selected.GetComponent<TransformComponent>();
            AssignParentTransform(transformComponent);
        }
    }

    glm::vec3 TransformationSystem::GetTransformCenter()
    {
        if (m_TransformationOrigin == TransformationOrigin::Cursor)
        {
            return m_Cursor->getPosition();
        }

        int count = 0;
        glm::vec3 center = { 0.0f,0.0f,0.0f };

        for (auto selected : m_SelectedEntities)
        {

            glm::vec3 position = selected.GetComponent<TransformComponent>().Transform->GetMatrix() * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
            center += position;
            count++;
        }


        center = glm::vec3(center.x / count, center.y / count, center.z / count);
        return center;
    }
}
