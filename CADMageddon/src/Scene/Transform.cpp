#include "Transform.h"
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtx\matrix_decompose.hpp>
#include <glm\gtc\quaternion.hpp>

namespace CADMageddon
{

    void Transform::SetParent(Ref<Transform> parent)
    {
        UnAssignParentTransform();
        AssignParentTransform(parent);
    }

    void Transform::AssignParentTransform(Ref<Transform> parent)
    {
        if (!parent)
        {
            return;
        }

        auto inverseParent = glm::inverse(parent->GetMatrix());
        auto newLocal = inverseParent * GetMatrix();

        glm::vec3 scale;
        glm::quat orientation;
        glm::vec3 translation;
        glm::vec3 skew;
        glm::vec4 perspective;

        glm::decompose(newLocal, scale, orientation, translation, skew, perspective);
        orientation = glm::conjugate(orientation);


        Parent = parent;
        glm::vec3 rotation = glm::degrees(glm::eulerAngles(orientation));
        Rotation = rotation;
        Translation = translation;
        Scale = scale;
    }

    void Transform::UnAssignParentTransform()
    {
        if (Parent == nullptr)
        {
            return;
        }

        auto newLocal = GetMatrix();

        glm::vec3 scale;
        glm::quat orientation;
        glm::vec3 translation;
        glm::vec3 skew;
        glm::vec4 perspective;

        glm::decompose(newLocal, scale, orientation, translation, skew, perspective);
        orientation = glm::conjugate(orientation);

        Parent = nullptr;
        glm::vec3 rotation = glm::degrees(glm::eulerAngles(orientation));
        Rotation = rotation;
        Translation = translation;
        Scale = scale;
    }

    glm::mat4 Transform::GetMatrix()
    {
        auto translationMatrix = glm::translate(glm::mat4(1.0f), Translation);
        auto rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(Rotation.x), glm::vec3(1.0f, 0.0f, 0.0f))
            * glm::rotate(glm::mat4(1.0f), glm::radians(Rotation.y), glm::vec3(0.0f, 1.0f, 0.0f))
            * glm::rotate(glm::mat4(1.0f), glm::radians(Rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        auto scaleMatrix = glm::scale(glm::mat4(1.0f), Scale);

        glm::mat4 parentMatrix = glm::mat4(1.0f);

        if (Parent)
        {
            parentMatrix = *Parent;
        }

        auto transform = parentMatrix * translationMatrix * rotationMatrix * scaleMatrix;
        return transform;;
    }

    const glm::mat4& Transform::GetMatrix() const
    {
        auto translationMatrix = glm::translate(glm::mat4(1.0f), Translation);
        auto rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(Rotation.x), glm::vec3(1.0f, 0.0f, 0.0f))
            * glm::rotate(glm::mat4(1.0f), glm::radians(Rotation.y), glm::vec3(0.0f, 1.0f, 0.0f))
            * glm::rotate(glm::mat4(1.0f), glm::radians(Rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        auto scaleMatrix = glm::scale(glm::mat4(1.0f), Scale);

        glm::mat4 parentMatrix = glm::mat4(1.0f);

        if (Parent)
        {
            parentMatrix = *Parent;
        }

        auto transform = parentMatrix * translationMatrix * rotationMatrix * scaleMatrix;
        return transform;
    }

}
