#pragma once
#include <glm\glm.hpp>

namespace CADMageddon
{
    class LineClipper
    {
    public:
        static void ClipLine(glm::vec2 p0, glm::vec2 p1, glm::vec2 xBoundary, glm::vec2 yBoundary, glm::vec2& intersection, glm::vec2& intersection2);
    private:
        typedef int OutCode;

        static OutCode GetOutCode(glm::vec2 p, glm::vec2 xBoundary, glm::vec2 yBoundary);
    };
}