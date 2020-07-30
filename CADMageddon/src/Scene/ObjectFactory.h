#pragma once
#include "Mesh.h"

namespace CADMageddon
{
    class ObjectFactory
    {
    public:
        static Mesh CreateTorusMesh(
            float majorRadius = 3.0f,
            float minorRadius = 1.0f,
            int majorRadiusCount = 30,
            int minorRadiusCount = 10);

        static Mesh CreateGridMesh(
            float width = 100.0f,
            float height = 100.0f
            /*float spacingHorizontal = 1.0f,
            float spacingVertical = 1.0f*/);
    };
}