#include "ObjectFactory.h"

#include <glm/gtc/constants.hpp>

namespace CADMageddon
{

    glm::vec3 GetTorusPoint(float majorRadius, float minorRadius, float u, float v)
    {
        glm::vec3 point;

        point.x = (majorRadius + minorRadius * cos(v)) * cos(u);
        point.y = (majorRadius + minorRadius * cos(v)) * sin(u);
        point.z = minorRadius * sin(v);

        return point;
    }

    Mesh ObjectFactory::CreateTorusMesh(
        float majorRadius,
        float minorRadius,
        int majorRadiusCount,
        int minorRadiusCount)
    {
        Mesh mesh;

        std::vector<glm::vec3> vertices;
        float uBegin = 0.0f;
        constexpr float uEnd = glm::two_pi<float>();
        float uDelta = (uEnd - uBegin) / majorRadiusCount;

        float vBegin = 0.0f;
        constexpr float vEnd = glm::two_pi<float>();
        float vDelta = (vEnd - vBegin) / minorRadiusCount;

        for (int uIndex = 0; uIndex < majorRadiusCount; uIndex++)
        {
            float actualU = uBegin + uIndex * uDelta;
            for (int vIndex = 0; vIndex < minorRadiusCount; vIndex++)
            {
                float actualV = vBegin + vIndex * vDelta;
                vertices.push_back(GetTorusPoint(majorRadius, minorRadius, actualU, actualV));
            }
        }

        std::vector<uint32_t> indices;
        for (int uIndex = 0; uIndex < majorRadiusCount; uIndex++)
        {
            for (int vIndex = 0; vIndex < minorRadiusCount; vIndex++)
            {
                int currentIndex = uIndex * minorRadiusCount + vIndex;
                int nextIndexInMajorCircle = currentIndex + minorRadiusCount;
                int nextIndexInMinorCircle = currentIndex + 1;

                int maxIndex = majorRadiusCount * minorRadiusCount - 1;
                int maxIndexInMinorCircle = uIndex * minorRadiusCount + minorRadiusCount - 1;

                if (nextIndexInMajorCircle > maxIndex)
                    nextIndexInMajorCircle -= maxIndex + 1;

                if (nextIndexInMinorCircle > maxIndexInMinorCircle)
                    nextIndexInMinorCircle -= minorRadiusCount;

                indices.push_back(currentIndex);
                indices.push_back(nextIndexInMinorCircle);
                indices.push_back(currentIndex);
                indices.push_back(nextIndexInMajorCircle);

            }
        }


        mesh.Vertices = vertices;
        mesh.Indices = indices;

        return mesh;
    }

    //glm::vec3 GetGridPoint(float majorRadius, float minorRadius, float u, float v)
    //{
    //    glm::vec3 point;

    //    point.x = (majorRadius + minorRadius * cos(v)) * cos(u);
    //    point.y = (majorRadius + minorRadius * cos(v)) * sin(u);
    //    point.z = minorRadius * sin(v);

    //    return point;
    //}

    Mesh ObjectFactory::CreateGridMesh(float width, float height)
    {
        Mesh mesh;
        //auto widthDelta = width / spacingHorizontal;
        //auto heightDelta = height / spacingVertical;

        auto halfWidth = width / 2.0f;
        auto halfHeight = height / 2.0f;

        std::vector<glm::vec3> vertices;
        for (auto u = -halfWidth; u <= halfWidth; u += 1.0f)
        {
            for (auto v = -halfHeight; v <= halfHeight; v += 1.0f)
            {
                vertices.push_back(glm::vec3(u, v, 0));
            }
        }

        int WidthCount = static_cast<int>(width) + 1;
        int HeightCount = static_cast<int>(height) + 1;

        std::vector<uint32_t> indices;

        for (int widthIndex = 0; widthIndex < WidthCount; widthIndex++)
        {
            for (int heightIndex = 0; heightIndex < HeightCount; heightIndex++)
            {
                int currentIndex = widthIndex * HeightCount + heightIndex;
                int nextRowIndex = currentIndex + HeightCount;
                int nextIndex = currentIndex + 1;
                if (nextIndex < widthIndex * HeightCount + HeightCount)
                {
                    indices.push_back(currentIndex);
                    indices.push_back(nextIndex);
                }
                if (nextRowIndex < WidthCount * HeightCount)
                {
                    indices.push_back(currentIndex);
                    indices.push_back(nextRowIndex);
                }
            }
        }

        mesh.Vertices = vertices;
        mesh.Indices = indices;

        return mesh;
    }
}