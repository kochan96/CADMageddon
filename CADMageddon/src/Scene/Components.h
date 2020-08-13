#pragma once
#include "cadpch.h"
#include "Core\Base.h"
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include "Point.h"
#include "ObjectFactory.h"
#include "Transform.h"

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
        Ref<Transform> Transform;

        TransformComponent()
        {
            Transform = CreateRef<CADMageddon::Transform>();
        }

        TransformComponent(const TransformComponent&) = default;
        TransformComponent(const glm::vec3& translation, const glm::vec3& rotation, const glm::vec3& scale)
        {
            Transform = CreateRef<CADMageddon::Transform>(translation, rotation, scale);
        }
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
        Ref<Point> Point;
        PointComponent()
        {
            Point = std::make_shared<CADMageddon::Point>();
        }

        PointComponent(const PointComponent&) = default;
        PointComponent(std::string name, const glm::vec3& position)
        {
            Point = std::make_shared<CADMageddon::Point>(position, name);
        }
    };

    struct HierarchyComponent
    {
        bool IsSelected = false;
        
        HierarchyComponent() = default;
        HierarchyComponent(const HierarchyComponent&) = default;
        HierarchyComponent(bool isSelected)
            : IsSelected(isSelected) {}

    };

    struct BezierC0Component
    {
        std::vector<Ref<Point>> ControlPoints;
        bool ShowPolygon = false;

        BezierC0Component() = default;
        BezierC0Component(const BezierC0Component&) = default;
        BezierC0Component(bool showPolygon, std::vector<Ref<Point>> points)
            : ShowPolygon(showPolygon), ControlPoints(points) {}
    };
}