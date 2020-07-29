#pragma once
#include "cadpch.h"
#include <glm\glm.hpp>


namespace CADMageddon
{
    struct Mesh
    {
        std::vector<glm::vec3> Vertices;
        std::vector<uint32_t> Indices;
    };
}