#pragma once
#include "cadpch.h"
#include <glm\glm.hpp>
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
        Mesh Mesh;
        float MajorRadius;
        float MinorRadius;
        int MajorRadiusCount;
        int MinorRadiusCount;

        TorusComponent() = default;
        TorusComponent(const TorusComponent&) = default;
        TorusComponent(float majorRadius, float minorRadius, int majorRadiusCount, int minorRadiusCount)
            :MajorRadius(majorRadius), MinorRadius(minorRadius),
            MajorRadiusCount(majorRadiusCount), MinorRadiusCount(minorRadiusCount),
            Mesh(ObjectFactory::CreateTorusMesh(majorRadius, minorRadius, majorRadiusCount, minorRadiusCount)) {}
    };
}