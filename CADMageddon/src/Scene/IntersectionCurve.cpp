#include "IntersectionCurve.h"
#include <glad\glad.h>

namespace CADMageddon
{
    Ref<InterpolatedCurve> IntersectionCurve::ConvertToInterpolated(std::string name)
    {
        auto interpolated = CreateRef<InterpolatedCurve>(name);
        for (int i = 0; i < m_IntersectionPoints.size(); i++)
        {
            auto point = CreateRef<Point>(m_IntersectionPoints[i].Location, name + "_Point_" + std::to_string(i));
            interpolated->AddControlPoint(point);
        }

        return interpolated;
    }

    void IntersectionCurve::CalculateTrimming(int lineCount, bool isFirst)
    {
        int index = isFirst ? 0 : 1;

        std::vector<glm::vec2> surfaceCoordinates;
        if (isFirst)
            std::transform(m_IntersectionPoints.begin(), m_IntersectionPoints.end(), std::back_inserter(surfaceCoordinates), [](IntersectionPoint point) {return glm::vec2(point.Coords.x, point.Coords.y); });
        else
            std::transform(m_IntersectionPoints.begin(), m_IntersectionPoints.end(), std::back_inserter(surfaceCoordinates), [](IntersectionPoint point) {return glm::vec2(point.Coords.z, point.Coords.w); });

        m_TexturePixels[index].clear();

        for (int i = 0; i < lineCount; i++)
        {
            float lineV = i / (float)(lineCount - 1);
            std::vector<float> intersections;
            GetIntersectionsAlongU(lineV, surfaceCoordinates, intersections);

            for (int offset = -1; offset <= 1; offset++)
            {
                for (int j = 1; j < intersections.size(); j += 2)
                {
                    auto intersectionLeft = intersections[j - 1] + offset;
                    auto intersectionRight = intersections[j] + offset;

                    float start = std::max(intersectionLeft, 0.0f);
                    float end = std::min(intersectionRight, 1.0f);

                    int startIndex = start * (lineCount - 1);
                    int endIndex = end * (lineCount - 1);

                    if (startIndex >= lineCount || endIndex < 0)
                    {
                        continue;
                    }

                    m_TexturePixels[index].push_back({ start,lineV });
                    m_TexturePixels[index].push_back({ end,lineV });
                }
            }
        }
    }

    void IntersectionCurve::GenerateTexture(int texNumber)
    {
        //needs optimization

        glGenTextures(1, &m_TextureIds[texNumber]);

        static const int TEX_SIZE = 8192;
        static unsigned char textureData[TEX_SIZE][TEX_SIZE];

        bool isFirst = texNumber == 0;
        CalculateTrimming(TEX_SIZE, isFirst);

        for (int i = 1; i < m_TexturePixels[texNumber].size(); i += 2)
        {
            auto row = m_TexturePixels[texNumber][i].y;
            auto start = m_TexturePixels[texNumber][i - 1].x;
            auto end = m_TexturePixels[texNumber][i].x;
            int rowIndex = row * (TEX_SIZE - 1);
            int startIndex = start * (TEX_SIZE - 1);
            int endIndex = end * (TEX_SIZE - 1) + 0.5f;

            for (int index = startIndex; index <= endIndex; index++)
            {
                textureData[rowIndex][index] = 255;
            }
        }


        glBindTexture(GL_TEXTURE_2D, m_TextureIds[texNumber]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, TEX_SIZE, TEX_SIZE, 0, GL_RED, GL_UNSIGNED_BYTE, textureData);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glBindTexture(GL_TEXTURE_2D, 0);
    }


    IntersectionCurve::IntersectionCurve(
        std::string name,
        std::vector<IntersectionPoint> points,
        Ref<SurfaceUV> s1,
        Ref<SurfaceUV> s2,
        IntersectionType intersectionType) :
        BaseObject(name),
        m_FirstSurface(s1),
        m_SecondSurface(s2),
        m_IntersectionPoints(points),
        m_ShowPlot(false)
    {

        if (intersectionType == IntersectionType::ClosedClosed || intersectionType == IntersectionType::ClosedOpen)
        {
            GenerateTexture(0);
        }

        if (intersectionType == IntersectionType::ClosedClosed || intersectionType == IntersectionType::OpenClosed)
        {
            GenerateTexture(1);
        }

        m_IntersectionType = intersectionType;
    }

    void IntersectionCurve::GetIntersectionsAlongU(float lineV, std::vector<glm::vec2> coords, std::vector<float>& intersections)
    {
        for (int i = 1; i < coords.size(); i++)
        {
            auto p1 = coords[i - 1];
            auto p2 = coords[i];

            GetIzolineIntersection(lineV, p1, p2, intersections);
            if (p1.x != 1.001f && p1.x != -0.001f && p1.y != 1.001f && p1.y != -0.001f &&
                p2.x != 1.001f && p2.x != -0.001f && p2.y != 1.001f && p2.y != -0.001f) {
                GetIzolineIntersection(lineV + 1, p1, p2, intersections);
                GetIzolineIntersection(lineV - 1, p1, p2, intersections);
            }
        }

        /*if (intersections.size() % 2 == 1)
            intersections.push_back(2);*/

        std::sort(intersections.begin(), intersections.end());
    }

    void IntersectionCurve::GetIntersectionsAlongV(float lineU, std::vector<glm::vec2> coords, std::vector<float>& intersections)
    {
        for (int i = 1; i < coords.size(); i++)
        {
            auto p1 = coords[i - 1];
            auto p2 = coords[i];

            p1 = glm::vec2(p1.y, p1.x);
            p2 = glm::vec2(p2.y, p2.x);

            GetIzolineIntersection(lineU, p1, p2, intersections);
            if (p1.x != 1.001f && p1.x != -0.001f && p1.y != 1.001f && p1.y != -0.001f &&
                p2.x != 1.001f && p2.x != -0.001f && p2.y != 1.001f && p2.y != -0.001f) {
                GetIzolineIntersection(lineU + 1, p1, p2, intersections);
                GetIzolineIntersection(lineU - 1, p1, p2, intersections);
            }

        }

        /*if (intersections.size() % 2 == 1)
            intersections.push_back(2);*/

        std::sort(intersections.begin(), intersections.end());
    }

    bool IntersectionCurve::CheckIfPixelInsideIntersections(float pixel, std::vector<float> intersections)
    {
        for (int i = 1; i < intersections.size(); i += 2)
        {
            auto intersectionLeft = intersections[i - 1];
            auto intersectionRight = intersections[i];

            if (intersectionLeft < pixel && pixel < intersectionRight)
            {
                return true;
            }

            /* if ((intersectionLeft - 1) <= pixel && pixel <= (intersectionRight - 1))
             {
                 return true;
             }*/

             /*if ((intersectionLeft + 1) <= pixel && pixel <= (intersectionRight + 1))
              {
                  return true;
              }*/
        }

        return false;
    }

    bool IntersectionCurve::GetIzolineIntersection(float line, glm::vec2 p1, glm::vec2 p2, std::vector<float>& intersections)
    {
        if (p1.y == line)
        {
            intersections.push_back(p1.x);
            intersections.push_back(p1.x);
            return true;
        }

        if (p2.y == line)
        {
            intersections.push_back(p2.x);
            intersections.push_back(p2.x);
            return line;
        }

        if ((p1.y - line) * (p2.y - line) < 0) //maybe <=
        {
            auto distance = p2 - p1;
            float x = p1.x + distance.x * (line - p1.y) / distance.y;
            intersections.push_back(x);
            return true;
        }

        return false;
    }


    Ref<IntersectionCurve> IntersectionCurve::Create(
        std::string name,
        std::vector<IntersectionPoint> points,
        Ref<SurfaceUV> s1,
        Ref<SurfaceUV> s2,
        IntersectionType intersectionType)
    {
        auto intersectionCurve = Ref<IntersectionCurve>(new IntersectionCurve(name, points, s1, s2, intersectionType));

        return intersectionCurve;
    }
}
