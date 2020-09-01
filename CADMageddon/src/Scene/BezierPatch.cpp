#include "BezierPatch.h"

namespace CADMageddon
{
    BezierPatch::BezierPatch(std::string name, int patchCountX, int patchCountY, int uDivisionCount, int vDivisionCount)
        :BaseObject(name), m_UDivisionCount(uDivisionCount), m_VDivisionCount(vDivisionCount), m_PatchCountX(patchCountX), m_PatchCountY(patchCountY)
    {
    }

    void BezierPatch::SetShowPoints(bool setShowPoints)
    {
        m_ShowPoints = setShowPoints;
        for (auto point : m_ControlPoints)
        {
            point->SetIsVisible(setShowPoints);
        }
    }

    Ref<BezierPatch> BezierPatch::CreateBezierPatch(
        std::string name,
        std::vector<Ref<Point>> controlPoints,
        int rowCount,
        int columnCount,
        int uDivisionCount,
        int vDivisionCount,
        bool isCylinder,
        bool showPolygon)
    {
        int PatchCountx = columnCount / 3;
        int PatchCounty = rowCount / 3;

        auto bezierPatch = CreateRef<BezierPatch>(name, PatchCountx, PatchCounty, uDivisionCount, vDivisionCount);
        bezierPatch->m_IsCylinder = isCylinder;
        bezierPatch->m_ShowPolygon = showPolygon;
        bezierPatch->m_ControlPoints = controlPoints;

        if (isCylinder)
            bezierPatch->GenerateCylinderIndices(PatchCountx, PatchCounty);
        else
            bezierPatch->GenerateRectIndices(PatchCountx, PatchCounty);

        bezierPatch->GenerateGridIndices(rowCount, columnCount);

        return bezierPatch;
    }

    Ref<BezierPatch> BezierPatch::CreateRectPatch(std::string name, glm::vec3 startPosition, int PatchCountx, int PatchCounty, float width, float height, int uDivisionCount, int vDivisionCount)
    {
        auto bezierPatch = CreateRef<BezierPatch>(name, PatchCountx, PatchCounty, uDivisionCount, vDivisionCount);
        bezierPatch->m_IsCylinder = false;
        bezierPatch->GenerateRectControlPoints(startPosition, PatchCountx, PatchCounty, width, height);
        bezierPatch->GenerateRectIndices(PatchCountx, PatchCounty);
        bezierPatch->GenerateGridIndices(PatchCounty * 3 + 1, PatchCountx * 3 + 1);
        return bezierPatch;
    }



    void BezierPatch::GenerateRectControlPoints(glm::vec3 startPosition, int PatchCountx, int PatchCounty, float width, float height)
    {
        int verticesCountX = PatchCountx * 3 + 1;
        int verticesCountY = PatchCounty * 3 + 1;

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
                auto point = CreateRef<Point>(position, m_Name + "Point_" + std::to_string(pointCount++));
                point->SetReferencedCount(1);
                m_ControlPoints.push_back(point);
            }
        }
    }

    void BezierPatch::GenerateRectIndices(int PatchCountx, int PatchCounty)
    {
        int verticesCountX = PatchCountx * 3 + 1;

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
                        m_Indices.push_back(index);
                    }
                }
            }
        }
    }

    Ref<BezierPatch> BezierPatch::CreateCyliderPatch(std::string name, glm::vec3 center, int PatchCountx, int PatchCounty, float radius, float height, int uDivisionCount, int vDivisionCount)
    {
        auto bezierPatch = CreateRef<BezierPatch>(name, PatchCountx, PatchCounty, uDivisionCount, vDivisionCount);
        bezierPatch->m_IsCylinder = true;
        bezierPatch->GenerateCylinderControlPoints(center, PatchCountx, PatchCounty, radius, height);
        bezierPatch->GenerateCylinderIndices(PatchCountx, PatchCounty);
        bezierPatch->GenerateGridIndices(PatchCounty * 3 + 1, PatchCountx * 3);
        return bezierPatch;
    }

    void BezierPatch::GenerateCylinderControlPoints(glm::vec3 center, int PatchCountx, int PatchCounty, float radius, float height)
    {
        int verticesCountX = PatchCountx * 3 + 1;
        int verticesCountY = PatchCounty * 3 + 1;

        float deltaAngle = glm::two_pi<float>() / (verticesCountX - 1);
        float deltaHeight = height / (verticesCountY - 1);

        verticesCountX--;
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
                auto point = CreateRef<Point>(position, m_Name + "Point_" + std::to_string(pointCount++));
                point->SetReferencedCount(1);
                m_ControlPoints.push_back(point);
            }
        }
    }


    void BezierPatch::GenerateCylinderIndices(int PatchCountx, int PatchCounty)
    {
        int verticesCountX = PatchCountx * 3;

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

                        m_Indices.push_back(index);
                    }
                }
            }
        }
    }

    void BezierPatch::GenerateGridIndices(int rowCount, int columnCount)
    {
        for (int i = 0; i < rowCount; i++)
        {
            for (int j = 0; j < columnCount; j++)
            {
                int row = i * columnCount;
                int index = row + j;
                if (index < row + columnCount - 1)
                {
                    m_GridIndices.push_back(index);
                    m_GridIndices.push_back(index + 1);
                }
                else if (m_IsCylinder)
                {
                    m_GridIndices.push_back(index);
                    m_GridIndices.push_back(row);
                }

                if (i < rowCount - 1)
                {
                    m_GridIndices.push_back(index);
                    m_GridIndices.push_back(index + columnCount);
                }
            }
        }
    }

    std::vector<uint32_t> BezierPatch::GetPatchIndices(float u, float v)
    {
        const int verticesCountX = m_IsCylinder ? m_PatchCountX * 3 : m_PatchCountX * 3 + 1;
        unsigned int startRow = std::min(int(v), m_PatchCountY - 1) * 3 * verticesCountX;
        unsigned int startColumn = std::min(int(u), m_PatchCountX - 1) * 3;
        std::vector<uint32_t> indices;

        int start = startRow + startColumn;
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                int index = startRow + i * verticesCountX + (j + startColumn) % verticesCountX;
                indices.push_back(index);
            }
        }
        return indices;
    }

    glm::vec4 BezierPatch::BernsteinBasis(float t)
    {
        float invT = 1.0f - t;

        return glm::vec4(
            invT * invT * invT,
            3.0f * t * invT * invT,
            3.0f * t * t * invT,
            t * t * t);
    }

    glm::vec4 BezierPatch::dBernsteinBasis(float t)
    {
        float invT = 1.0f - t;

        return glm::vec4(
            -3 * invT * invT,
            3 * invT * invT - 6 * t * invT,
            6 * t * invT - 3.0f * t * t,
            3 * t * t);
    }

    glm::vec3 BezierPatch::GetPointAt(float u, float v)
    {
        u = std::clamp(u, 0.0f, 1.0f);
        v = m_IsCylinder ? v - std::floorf(v) : glm::clamp(v, 0.0f, 1.0f);

        u = u * m_PatchCountX;
        v = v * m_PatchCountY;
        auto patchIndices = GetPatchIndices(u, v);

        u -= std::min(int(u), m_PatchCountX - 1);
        v -= std::min(int(v), m_PatchCountY - 1);

        glm::vec3 point;

        auto basisU = BernsteinBasis(u);
        auto basisV = BernsteinBasis(v);

        std::vector<glm::vec3> controlPointsPosition;
        controlPointsPosition.reserve(m_ControlPoints.size());
        std::transform(m_ControlPoints.begin(), m_ControlPoints.end(), std::back_inserter(controlPointsPosition), [](Ref<Point> p) {return p->GetPosition(); });

        point = basisU.x * (basisV.x * controlPointsPosition[patchIndices[0]]
            + basisV.y * controlPointsPosition[patchIndices[4]]
            + basisV.z * controlPointsPosition[patchIndices[8]]
            + basisV.w * controlPointsPosition[patchIndices[12]]);

        point += basisU.y * (basisV.x * controlPointsPosition[patchIndices[1]]
            + basisV.y * controlPointsPosition[patchIndices[5]]
            + basisV.z * controlPointsPosition[patchIndices[9]]
            + basisV.w * controlPointsPosition[patchIndices[13]]);

        point += basisU.z * (basisV.x * controlPointsPosition[patchIndices[2]]
            + basisV.y * controlPointsPosition[patchIndices[6]]
            + basisV.z * controlPointsPosition[patchIndices[10]]
            + basisV.w * controlPointsPosition[patchIndices[14]]);

        point += basisU.w * (basisV.x * controlPointsPosition[patchIndices[3]]
            + basisV.y * controlPointsPosition[patchIndices[7]]
            + basisV.z * controlPointsPosition[patchIndices[11]]
            + basisV.w * controlPointsPosition[patchIndices[15]]);

        return point;
    }

    glm::vec3 BezierPatch::GetTangentUAt(float u, float v)
    {
        u = std::clamp(u, 0.0f, 1.0f);
        v = m_IsCylinder ? v - std::floorf(v) : glm::clamp(v, 0.0f, 1.0f);

        u = u * m_PatchCountX;
        v = v * m_PatchCountY;
        auto patchIndices = GetPatchIndices(u, v);

        u -= std::min(int(u), m_PatchCountX - 1);
        v -= std::min(int(v), m_PatchCountY - 1);

        glm::vec3 point;
        auto basisU = dBernsteinBasis(u);
        auto basisV = BernsteinBasis(v);

        std::vector<glm::vec3> controlPointsPosition;
        controlPointsPosition.reserve(m_ControlPoints.size());
        std::transform(m_ControlPoints.begin(), m_ControlPoints.end(), std::back_inserter(controlPointsPosition), [](Ref<Point> p) {return p->GetPosition(); });

        point = basisU.x * (basisV.x * controlPointsPosition[patchIndices[0]]
            + basisV.y * controlPointsPosition[patchIndices[4]]
            + basisV.z * controlPointsPosition[patchIndices[8]]
            + basisV.w * controlPointsPosition[patchIndices[12]]);

        point += basisU.y * (basisV.x * controlPointsPosition[patchIndices[1]]
            + basisV.y * controlPointsPosition[patchIndices[5]]
            + basisV.z * controlPointsPosition[patchIndices[9]]
            + basisV.w * controlPointsPosition[patchIndices[13]]);

        point += basisU.z * (basisV.x * controlPointsPosition[patchIndices[2]]
            + basisV.y * controlPointsPosition[patchIndices[6]]
            + basisV.z * controlPointsPosition[patchIndices[10]]
            + basisV.w * controlPointsPosition[patchIndices[14]]);

        point += basisU.w * (basisV.x * controlPointsPosition[patchIndices[3]]
            + basisV.y * controlPointsPosition[patchIndices[7]]
            + basisV.z * controlPointsPosition[patchIndices[11]]
            + basisV.w * controlPointsPosition[patchIndices[15]]);

        return point;
    }

    glm::vec3 BezierPatch::GetTangentVAt(float u, float v)
    {
        u = std::clamp(u, 0.0f, 1.0f);
        v = m_IsCylinder ? v - std::floorf(v) : glm::clamp(v, 0.0f, 1.0f);

        u = u * m_PatchCountX;
        v = v * m_PatchCountY;
        auto patchIndices = GetPatchIndices(u, v);

        u -= std::min(int(u), m_PatchCountX - 1);
        v -= std::min(int(v), m_PatchCountY - 1);

        glm::vec3 point;
        auto basisU = BernsteinBasis(u);
        auto basisV = dBernsteinBasis(v);

        std::vector<glm::vec3> controlPointsPosition;
        controlPointsPosition.reserve(m_ControlPoints.size());
        std::transform(m_ControlPoints.begin(), m_ControlPoints.end(), std::back_inserter(controlPointsPosition), [](Ref<Point> p) {return p->GetPosition(); });

        point = basisU.x * (basisV.x * controlPointsPosition[patchIndices[0]]
            + basisV.y * controlPointsPosition[patchIndices[4]]
            + basisV.z * controlPointsPosition[patchIndices[8]]
            + basisV.w * controlPointsPosition[patchIndices[12]]);

        point += basisU.y * (basisV.x * controlPointsPosition[patchIndices[1]]
            + basisV.y * controlPointsPosition[patchIndices[5]]
            + basisV.z * controlPointsPosition[patchIndices[9]]
            + basisV.w * controlPointsPosition[patchIndices[13]]);

        point += basisU.z * (basisV.x * controlPointsPosition[patchIndices[2]]
            + basisV.y * controlPointsPosition[patchIndices[6]]
            + basisV.z * controlPointsPosition[patchIndices[10]]
            + basisV.w * controlPointsPosition[patchIndices[14]]);

        point += basisU.w * (basisV.x * controlPointsPosition[patchIndices[3]]
            + basisV.y * controlPointsPosition[patchIndices[7]]
            + basisV.z * controlPointsPosition[patchIndices[11]]
            + basisV.w * controlPointsPosition[patchIndices[15]]);

        return point;
    }

    float BezierPatch::GetMinU() const
    {
        return 0.0f;
    }

    float BezierPatch::GetMaxU() const
    {
        return 1.0f;
    }

    float BezierPatch::GetMinV() const
    {
        return 0.0f;
    }

    float BezierPatch::GetMaxV() const
    {
        return 1.0f;
    }
}
