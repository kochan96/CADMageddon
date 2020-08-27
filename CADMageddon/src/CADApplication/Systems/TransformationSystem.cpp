#include "TransformationSystem.h"
#include <glm\gtc\quaternion.hpp>
#include <glm\glm.hpp>
#include <glm\gtx\matrix_decompose.hpp>
#include "Gizmos\Gizmo.h"
#include "Cursor3D.h"
#include "imgui.h"

namespace CADMageddon
{

    ///TODO: fix rotation issues

    TransformationSystem::TransformationSystem(Ref<Cursor3D> cursor)
        :m_Cursor(cursor)
    {
        m_TransformationParent = CreateRef<Transform>();
        m_TransformationMode = TransformationMode::Translation;
        m_TransformationOrigin = TransformationOrigin::Center;
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

        if (m_TransformationOrigin == TransformationOrigin::Cursor && m_TransformationMode == TransformationMode::Translation)
        {
            RecalculateParentAndChildrenTransform();
        }
    }

    void TransformationSystem::RenderImGui()
    {
        auto& transform = GetTransformToModify();

        auto position = transform.Translation;
        if (ImGui::DragFloat3("Position", &position.x, 0.1f))
        {
            transform.Translation = position;
            if (m_TransformationOrigin == TransformationOrigin::Cursor)
            {
                RecalculateParentAndChildrenTransform();
            }
        }

        auto rotation = transform.Rotation;
        if (ImGui::DragFloat3("Rotation", &rotation.x))
        {
            transform.Rotation = rotation;
        }

        auto scale = transform.Scale;
        if (ImGui::DragFloat3("Scale", &scale.x, 0.1f))
        {
            transform.Scale = scale;
        }
    }

    void TransformationSystem::AddToSelected(Ref<Transform> entity)
    {
        m_SelectedEntities.push_back(entity);
        RecalculateParentAndChildrenTransform();
    }

    void TransformationSystem::RemoveFromSelected(Ref<Transform> entity)
    {
        auto it = std::find(m_SelectedEntities.begin(), m_SelectedEntities.end(), entity);
        if (it != m_SelectedEntities.end())
        {
            UnAssignParentTransform(**it);
            m_SelectedEntities.erase(it);
            RecalculateParentAndChildrenTransform();
        }
    }

    void TransformationSystem::ClearSelection()
    {
        for (auto selected : m_SelectedEntities)
        {
            UnAssignParentTransform(*selected);
        }

        m_SelectedEntities.clear();
    }

    Transform& TransformationSystem::GetTransformToModify()
    {

        if (m_SelectedEntities.size() == 1 && m_TransformationOrigin == TransformationOrigin::Center)
        {
            return *m_SelectedEntities[0];
        }
        else
        {
            return *m_TransformationParent;
        }
    }

    void TransformationSystem::AssignParentTransform(Transform& transform)
    {
        transform.Parent = m_TransformationParent;
        transform.Translation -= m_TransformationParent->Translation;

    }

    void TransformationSystem::UnAssignParentTransform(Transform& transform)
    {
        glm::vec3 parentTranslation = transform.GetMatrix() * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        transform.Translation = parentTranslation;
        transform.Parent = nullptr;
    }

    void TransformationSystem::RecalculateParentAndChildrenTransform()
    {
        if (m_SelectedEntities.empty())
        {
            return;
        }

        for (auto selected : m_SelectedEntities)
        {
            UnAssignParentTransform(*selected);
        }

        if (m_TransformationOrigin == TransformationOrigin::Center && m_SelectedEntities.size() == 1)
        {
            return;
        }

        auto center = GetTransformCenter();
        m_TransformationParent->Translation = center;
        m_TransformationParent->Rotation = glm::vec3(0.0f, 0.0f, 0.0f);
        m_TransformationParent->Scale = glm::vec3(1.0f, 1.0f, 1.0f);

        for (auto selected : m_SelectedEntities)
        {
            AssignParentTransform(*selected);
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

            glm::vec3 position = selected->GetMatrix() * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
            center += position;
            count++;
        }


        center = glm::vec3(center.x / count, center.y / count, center.z / count);
        return center;
    }
}
