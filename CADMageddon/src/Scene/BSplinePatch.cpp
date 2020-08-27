#include "BSplinePatch.h"

namespace CADMageddon
{
    BSplinePatch::BSplinePatch(std::string name, int patchCountX, int patchCountY, int uDivisionCount, int vDivisionCount)
        :m_Name(name), m_UDivisionCount(uDivisionCount), m_VDivisionCount(vDivisionCount), m_PatchCountX(patchCountX), m_PatchCountY(patchCountY)
    {
    }

    Ref<BSplinePatch> BSplinePatch::CreateBSplinePatch(
        std::string name,
        std::vector<Ref<Point>> controlPoints,
        std::vector<uint32_t> indices,
        std::vector<uint32_t> gridIndices,
        int PatchCountx,
        int PatchCounty,
        int uDivisionCount,
        int vDivisionCount,
        bool isCylinder,
        bool showPolygon)
    {
        auto bSplinePatch = CreateRef<BSplinePatch>(name, PatchCountx, PatchCounty, uDivisionCount, vDivisionCount);
        bSplinePatch->m_ControlPoints = controlPoints;
        bSplinePatch->m_Indices = indices;
        bSplinePatch->m_GridIndices = gridIndices;
        bSplinePatch->m_IsCylinder = isCylinder;
        bSplinePatch->m_ShowPolygon = showPolygon;

        return bSplinePatch;
    }

    Ref<BSplinePatch> BSplinePatch::CreateRectPatch(
        std::string name,
        glm::vec3 startPosition,
        int PatchCountx,
        int PatchCounty,
        float width,
        float height,
        int uDivisionCount,
        int vDivisionCount)
    {
        auto bSplinePatch = CreateRef<BSplinePatch>(name, PatchCountx, PatchCounty, uDivisionCount, vDivisionCount);
        bSplinePatch->GenerateRectControlPoints(startPosition, PatchCountx, PatchCounty, width, height);
        bSplinePatch->m_IsCylinder = false;
        return bSplinePatch;
    }

    void BSplinePatch::GenerateRectControlPoints(glm::vec3 startPosition, int PatchCountx, int PatchCounty, float width, float height)
    {
        int verticesCountX = PatchCountx + 3;
        int verticesCountY = PatchCounty + 3;

        float deltaWidth = width / (verticesCountX - 1);
        float deltaHeight = height / (verticesCountY - 1);
        int pointCount = 0;

        for (int i = 0; i < verticesCountY; i++)
        {
            for (int j = 0; j < verticesCountX; j++)
            {
                float u = j * deltaWidth;
                float v = i * deltaHeight;

                glm::vec3 position = startPosition + glm::vec3(u, 0.0f, 0.0f) + glm::vec3(0.0f, v, 0.0f);
                m_ControlPoints.push_back(CreateRef<Point>(position, m_Name + "Point_" + std::to_string(pointCount++)));
            }
        }


        for (int j = 0; j < PatchCounty; j++)
        {
            int startRow = j * verticesCountX;
            for (int i = 0; i < PatchCountx; i++)
            {
                int start = startRow + i;
                for (int k = 0; k < 4; k++)
                {
                    for (int l = 0; l < 4; l++)
                    {
                        int index = start + k * verticesCountX + l;
                        m_Indices.push_back(index);
                    }
                }
            }
        }

        for (int i = 0; i < verticesCountY; i++)
        {
            for (int j = 0; j < verticesCountX; j++)
            {
                int row = i * verticesCountX;
                int index = row + j;
                if (index < row + verticesCountX - 1)
                {
                    m_GridIndices.push_back(index);
                    m_GridIndices.push_back(index + 1);
                }

                if (i < verticesCountY - 1)
                {
                    m_GridIndices.push_back(index);
                    m_GridIndices.push_back(index + verticesCountX);
                }
            }
        }
    }

    Ref<BSplinePatch> BSplinePatch::CreateCyliderPatch(std::string name, glm::vec3 center, int PatchCountx, int PatchCounty, float radius, float height, int uDivisionCount, int vDivisionCount)
    {
        auto bSplinePatch = CreateRef<BSplinePatch>(name, PatchCountx, PatchCounty, uDivisionCount, vDivisionCount);
        bSplinePatch->GenerateCylinderControlPoints(center, PatchCountx, PatchCounty, radius, height);
        bSplinePatch->m_IsCylinder = true;
        return bSplinePatch;
    }

    void BSplinePatch::GenerateCylinderControlPoints(glm::vec3 center, int PatchCountx, int PatchCounty, float radius, float height)
    {
        int verticesCountX = PatchCountx;
        int verticesCountY = PatchCounty + 3;

        float deltaAngle = glm::two_pi<float>() / verticesCountX;

        float deltaHeight = height / (verticesCountY - 1);

        int pointCount = 0;

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
                m_ControlPoints.push_back(CreateRef<Point>(position, m_Name + "Point_" + std::to_string(pointCount++)));
            }
        }

        for (int j = 0; j < PatchCounty; j++)
        {
            int startRow = j * verticesCountX;
            for (int i = 0; i < PatchCountx; i++)
            {
                int start = startRow + i;
                for (int k = 0; k < 4; k++)
                {
                    for (int l = 0; l < 4; l++)
                    {
                        int index = (k + j) * verticesCountX + (l + i) % verticesCountX;
                        m_Indices.push_back(index);
                    }
                }
            }
        }

        for (int i = 0; i < verticesCountY; i++)
        {
            for (int j = 0; j < verticesCountX; j++)
            {
                int row = i * verticesCountX;
                int index = row + j;
                if (index < row + verticesCountX - 1)
                {
                    m_GridIndices.push_back(index);
                    m_GridIndices.push_back(index + 1);
                }
                else
                {
                    m_GridIndices.push_back(index);
                    m_GridIndices.push_back(row);
                }

                if (i < verticesCountY - 1)
                {
                    m_GridIndices.push_back(index);
                    m_GridIndices.push_back(index + verticesCountX);
                }
            }
        }
    }
}
