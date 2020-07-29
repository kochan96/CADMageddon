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
    };
}