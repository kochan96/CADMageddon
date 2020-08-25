#include "BezierPatch.h"

namespace CADMageddon
{
    BezierPatch::BezierPatch(std::string name, int patchCountX, int patchCountY, int uDivisionCount, int vDivisionCount)
        :m_Name(name), m_UDivisionCount(uDivisionCount), m_VDivisionCount(vDivisionCount), m_PatchCountX(patchCountX), m_PatchCountY(patchCountY)
    {
    }

    Ref<BezierPatch> BezierPatch::CreateRectPatch(std::string name, glm::vec3 startPosition, int PatchCountx, int PatchCounty, float width, float height, int uDivisionCount, int vDivisionCount)
    {
        auto bezierPatch = CreateRef<BezierPatch>(name, uDivisionCount, vDivisionCount);
        bezierPatch->GenerateRectControlPoints(startPosition, PatchCountx, PatchCounty, width, height);
        return bezierPatch;
    }

    void BezierPatch::GenerateRectControlPoints(glm::vec3 startPosition, int PatchCountx, int PatchCounty, float width, float height)
    {
        int verticesCountX = (PatchCountx - 1) * 3 + 4;
        int verticesCountY = (PatchCounty - 1) * 3 + 4;

        float deltaWidth = width / (verticesCountX - 1);
        float deltaHeight = height / (verticesCountY - 1);

        for (int i = 0; i < verticesCountY; i++)
        {
            for (int j = 0; j < verticesCountX; j++)
            {
                float u = j * deltaWidth;
                float v = i * deltaHeight;

                glm::vec3 position = startPosition + glm::vec3(u, 0.0f, 0.0f) + glm::vec3(0.0f, v, 0.0f);
                m_ControlPoints.push_back(CreateRef<Point>(position));
            }
        }


        for (int j = 0; j < PatchCounty; j++)
        {
            int startRow = j * 3 * verticesCountX;
            for (int i = 0; i < PatchCountx; i++)
            {
                int start = startRow + i * 3;
                for (int k = 0; k < 4; k++)
                {
                    for (int l = 0; l < 4; l++)
                    {
                        int index = start + k * verticesCountX + l;
                        m_RenderingControlPoints.push_back(m_ControlPoints[index]);
                    }
                }
            }
        }
    }

    Ref<BezierPatch> BezierPatch::CreateCyliderPatch(std::string name, glm::vec3 center, int PatchCountx, int PatchCounty, float radius, float height, int uDivisionCount, int vDivisionCount)
    {
        auto bezierPatch = CreateRef<BezierPatch>(name, PatchCountx, PatchCounty, uDivisionCount, vDivisionCount);
        bezierPatch->GenerateCylinderControlPoints(center, PatchCountx, PatchCounty, radius, height);
        return bezierPatch;
    }

    void BezierPatch::GenerateCylinderControlPoints(glm::vec3 center, int PatchCountx, int PatchCounty, float radius, float height)
    {
        int verticesCountX = (PatchCountx - 1) * 3 + 4;
        int verticesCountY = (PatchCounty - 1) * 3 + 4;

        float deltaAngle = glm::two_pi<float>() / (verticesCountX - 1);
        float deltaHeight = height / (verticesCountY - 1);

        verticesCountX--;

        for (int i = 0; i < verticesCountY; i++)
        {
            for (int j = 0; j < verticesCountX; j++)
            {
                float u = j * deltaAngle;
                float v = i * deltaHeight;

                glm::vec3 position = center;
                position.x += radius * cos(u);
                position.z += radius * sin(u);
                position.y += v;
                m_ControlPoints.push_back(CreateRef<Point>(position));
            }
        }


        for (int j = 0; j < PatchCounty; j++)
        {
            int startRow = (j * 3 * verticesCountX);
            for (int i = 0; i < PatchCountx; i++)
            {
                int start = startRow + i * 3;
                for (int k = 0; k < 4; k++)
                {
                    for (int l = 0; l < 4; l++)
                    {
                        int index = start + k * verticesCountX + l;
                        if (i == (PatchCountx - 1) && l == 3)
                        {
                            index = startRow + k * verticesCountX;
                        }

                        m_RenderingControlPoints.push_back(m_ControlPoints[index]);
                    }
                }
            }
        }
    }
}
