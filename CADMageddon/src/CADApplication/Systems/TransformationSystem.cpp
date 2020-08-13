#include "TransformationSystem.h"
#include "Scene\Entity.h"
#include <glm\gtc\quaternion.hpp>
#include <glm\glm.hpp>
#include <glm\gtx\matrix_decompose.hpp>
#include "Gizmos\Gizmo.h"

namespace CADMageddon
{

    ///TODO: fix rotation issues

    TransformationSystem::TransformationSystem()
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
                Gizmo::Manipulate(GizmoMode::Translation, transformToManipulate, camera.GetViewProjectionMatrix(), ndcMousePosition);
                break;
            case TransformationMode::Rotation:
                Gizmo::Manipulate(GizmoMode::Rotation, transformToManipulate, camera.GetViewProjectionMatrix(), ndcMousePosition);
                break;
            case TransformationMode::Scaling:
                Gizmo::Manipulate(GizmoMode::Scale, transformToManipulate, camera.GetViewProjectionMatrix(), ndcMousePosition);
                break;
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

        if (m_SelectedEntities.size() == 1)
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
        auto inverseParent = glm::translate(glm::mat4(1.0f), -m_TransformationParent->Translation);
        auto newLocal = inverseParent * transform.GetMatrix();

        glm::vec3 scale;
        glm::quat orientation;
        glm::vec3 translation;
        glm::vec3 skew;
        glm::vec4 perspective;

        glm::decompose(newLocal, scale, orientation, translation, skew, perspective);
        orientation = glm::conjugate(orientation);


        transform.Parent = m_TransformationParent;
        glm::vec3 rotation = glm::degrees(glm::eulerAngles(orientation));
        transform.Rotation = rotation;
        transform.Translation = translation;
        transform.Scale = scale;
    }

    void TransformationSystem::UnAssignParentTransform(TransformComponent& transform)
    {
        if (transform.Parent == nullptr)
        {
            return;
        }

        auto newLocal = transform.GetMatrix();

        glm::vec3 scale;
        glm::quat orientation;
        glm::vec3 translation;
        glm::vec3 skew;
        glm::vec4 perspective;

        glm::decompose(newLocal, scale, orientation, translation, skew, perspective);
        orientation = glm::conjugate(orientation);

        transform.Parent = nullptr;
        glm::vec3 rotation = glm::degrees(glm::eulerAngles(orientation));
        transform.Rotation = rotation;
        transform.Translation = translation;
        transform.Scale = scale;
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

        if (m_SelectedEntities.size() == 1)
        {
            return;
        }

        auto center = GetTransformCenter();
        m_TransformationParent->Translation = center;
        m_TransformationParent->Rotation = glm::vec3(0.0f, 0.0f, 0.0f);
        m_TransformationParent->Scale = glm::vec3(1.0f, 1.0f, 1.0f);

        for (auto selected : m_SelectedEntities)
        {
            auto& transformComponent = selected.GetComponent<TransformComponent>();
            AssignParentTransform(transformComponent);
        }
    }

    glm::vec3 TransformationSystem::GetTransformCenter()
    {
        int count = 0;
        glm::vec3 center = { 0.0f,0.0f,0.0f };

        for (auto selected : m_SelectedEntities)
        {

            glm::vec3 position = selected.GetComponent<TransformComponent>().GetMatrix() * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
            center += position;
            count++;
        }


        center = glm::vec3(center.x / count, center.y / count, center.z / count);
        return center;
    }
}
