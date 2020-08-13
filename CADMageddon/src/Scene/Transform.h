#pragma once
#include <glm\glm.hpp>
#include <Core\Base.h>

namespace CADMageddon
{
    class Transform
    {

    public:
        Transform() = default;
        Transform(const Transform&) = default;
        Transform(const glm::vec3& translation, const glm::vec3& rotation, const glm::vec3& scale)
            : Translation(translation), Rotation(rotation), Scale(scale) {}

        glm::vec3 Translation = { 0.0f,0.0f,0.0f };
        glm::vec3 Rotation = { 0.0f,0.0f,0.0f }; //TODO maybe quaternion or at least keep it in radians
        glm::vec3 Scale = { 1.0f,1.0f,1.0f };

        void SetParent(Ref<Transform> parent);

        operator glm::mat4()
        {
            return GetMatrix();
        }
        operator const glm::mat4() const
        {
            return GetMatrix();
        }

        glm::mat4 GetMatrix();
        const glm::mat4& GetMatrix() const;

    private:
       
        void UnAssignParentTransform();
        void AssignParentTransform(Ref<Transform> transform);

    private:
        Ref<Transform> Parent = nullptr;
    };
}