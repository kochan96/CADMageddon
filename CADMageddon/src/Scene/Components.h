#pragma once
#include "cadpch.h"
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include "ObjectFactory.h"

namespace CADMageddon
{
    struct NameComponent
    {
        std::string Name;

        NameComponent() = default;
        NameComponent(const NameComponent&) = default;
        NameComponent(const std::string& name)
            : Name(name) {}
    };

    struct TorusComponent
    {
        float MajorRadius;
        float MinorRadius;
        int MajorRadiusCount;
        int MinorRadiusCount;
        Mesh Mesh;

        TorusComponent() :TorusComponent(3.0f, 1.0f, 30, 10) {}
        TorusComponent(const TorusComponent&) = default;
        TorusComponent(float majorRadius, float minorRadius, int majorRadiusCount, int minorRadiusCount)
            :MajorRadius(majorRadius), MinorRadius(minorRadius),
            MajorRadiusCount(majorRadiusCount), MinorRadiusCount(minorRadiusCount),
            Mesh(ObjectFactory::CreateTorusMesh(majorRadius, minorRadius, majorRadiusCount, minorRadiusCount)) {}
    };

    struct TransformComponent
    {
        glm::vec3 Translation = { 0.0f,0.0f,0.0f };
        glm::vec3 Rotation = { 0.0f,0.0f,0.0f }; //TODO maybe quaternion or at least keep it in radians
        glm::vec3 Scale = { 1.0f,1.0f,1.0f };

        operator glm::mat4()
        {
            auto translationMatrix = glm::translate(glm::mat4(1.0f), Translation);
            auto rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(Rotation.y), glm::vec3(0.0f, 1.0f, 0.0f))
                * glm::rotate(glm::mat4(1.0f), glm::radians(Rotation.x), glm::vec3(1.0f, 0.0f, 0.0f))
                * glm::rotate(glm::mat4(1.0f), glm::radians(Rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
            auto scaleMatrix = glm::scale(glm::mat4(1.0f), Scale);

            auto transform = translationMatrix * rotationMatrix * scaleMatrix;
            return transform;
        }

        operator const glm::mat4() const
        {
            auto translationMatrix = glm::translate(glm::mat4(1.0f), Translation);
            auto rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(Rotation.y), glm::vec3(0.0f, 1.0f, 0.0f))
                * glm::rotate(glm::mat4(1.0f), glm::radians(Rotation.x), glm::vec3(1.0f, 0.0f, 0.0f))
                * glm::rotate(glm::mat4(1.0f), glm::radians(Rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
            auto scaleMatrix = glm::scale(glm::mat4(1.0f), Scale);

            auto transform = translationMatrix * rotationMatrix * scaleMatrix;
            return transform;
        }

        TransformComponent() = default;
        TransformComponent(const TransformComponent&) = default;
        TransformComponent(const glm::vec3& translation, const glm::vec3& rotation, const glm::vec3& scale)
            : Translation(translation), Rotation(rotation), Scale(scale) {}
    };


    struct ColorComponent
    {
        glm::vec4 Color;

        ColorComponent() = default;
        ColorComponent(const ColorComponent&) = default;
        ColorComponent(const glm::vec4& color)
            : Color(color) {}
    };
}