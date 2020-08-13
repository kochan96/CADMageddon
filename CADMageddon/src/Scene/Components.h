#pragma once
#include "cadpch.h"
#include "Core\Base.h"
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include "Point.h"
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

        std::vector<Point> Points;
        std::vector<uint32_t> Indices;

        TorusComponent() :TorusComponent(3.0f, 1.0f, 30, 10) {}
        TorusComponent(const TorusComponent&) = default;
        TorusComponent(float majorRadius, float minorRadius, int majorRadiusCount, int minorRadiusCount)
            :MajorRadius(majorRadius), MinorRadius(minorRadius),
            MajorRadiusCount(majorRadiusCount), MinorRadiusCount(minorRadiusCount)
        {
            Mesh mesh = ObjectFactory::CreateTorusMesh(majorRadius, minorRadius, MajorRadiusCount, MinorRadiusCount);
            Points.insert(Points.begin(), mesh.Vertices.begin(), mesh.Vertices.end());
            Indices = mesh.Indices;
        }
    };

    struct TransformComponent
    {
        glm::vec3 Translation = { 0.0f,0.0f,0.0f };
        glm::vec3 Rotation = { 0.0f,0.0f,0.0f }; //TODO maybe quaternion or at least keep it in radians
        glm::vec3 Scale = { 1.0f,1.0f,1.0f };

        Ref<TransformComponent> Parent = nullptr;

        glm::mat4 GetMatrix()
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

        const glm::mat4& GetMatrix() const
        {
            auto translationMatrix = glm::translate(glm::mat4(1.0f), Translation);
            auto rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(Rotation.y), glm::vec3(0.0f, 1.0f, 0.0f))
                * glm::rotate(glm::mat4(1.0f), glm::radians(Rotation.x), glm::vec3(1.0f, 0.0f, 0.0f))
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

        operator glm::mat4()
        {
            return GetMatrix();
        }
        operator const glm::mat4() const
        {
            return GetMatrix();
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

    struct PointComponent
    {
        Point Point;
        PointComponent() = default;
        PointComponent(const PointComponent&) = default;
        PointComponent(std::string name, const glm::vec3& position)
            : Point(position, name) {}
    };

    struct HierarchyComponent
    {
        bool IsSelected = false;
        HierarchyComponent() = default;
        HierarchyComponent(const HierarchyComponent&) = default;
        HierarchyComponent(bool isSelected)
            : IsSelected(isSelected) {}
    };
}