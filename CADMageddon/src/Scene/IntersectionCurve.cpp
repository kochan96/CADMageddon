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

    void IntersectionCurve::GenerateTextures()
    {
        /*glGenTextures(2, m_TexrureIds);
        static const int TEX_SIZE = 8192;
        static unsigned char textureData[TEX_SIZE][TEX_SIZE];
        for (int i = 0; i < 2; i++)
        {
            CalculateTrimming(TEX_SIZE, true, i > 0 ? true : false);
            for (int i = 0; i < TEX_SIZE; i++)
            {
                bool isIn = false;
                int vecInx = 0;
                std::vector<float>& line = grid[1 + 2 * !k][i];
                for (int j = 0; j < TEX_SIZE; j++)
                {
                    float linePix = (j + 1) / (float)(TEX_SIZE + 1) + 1.f / 8192.f;
                    while (vecInx < line.size() && line[vecInx] < linePix)
                    {
                        isIn = !isIn;
                        vecInx++;
                    }

                    textureData[i][j] = isIn ? 0 : 255;
                }
            }

            glBindTexture(GL_TEXTURE_2D, m_TexrureIds[i]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, TEX_SIZE, TEX_SIZE, 0, GL_RED, GL_UNSIGNED_BYTE, textureData);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glBindTexture(GL_TEXTURE_2D, 0);
        }*/
    }

    bool IntersectionCurve::GetIzolineIntersection(float line, glm::vec2 p1, glm::vec2 p2, bool isReversed, float& intersection)
    {
        if (isReversed)
        {
            std::swap(p1.x, p1.y);
            std::swap(p2.x, p2.y);
        }

        if ((p1.y - line) * (p2.y - line) < 0) {
            float x = (p2.y * p1.x - line * p1.x + p2.x * line - p2.x * p1.y) /
                (p2.y - p1.y);
            if (x < 0 || x > 1)
                x -= std::floorf(x);
            intersection = (x);

            return true;
        }

        return false;
    }

    std::vector<glm::vec2> IntersectionCurve::CalculateTrimming(int lineCount, bool alongU, bool isFirst)
    {
        return std::vector<glm::vec2>();
    }

    IntersectionCurve::IntersectionCurve(
        std::string name,
        std::vector<IntersectionPoint> points,
        Ref<SurfaceUV> s1,
        Ref<SurfaceUV> s2,
        bool isClosed) :
        BaseObject(name),
        m_FirstSurface(s1),
        m_SecondSurface(s2),
        m_IntersectionPoints(points),
        m_IsClosed(isClosed) {}

    Ref<IntersectionCurve> IntersectionCurve::Create(
        std::string name,
        std::vector<IntersectionPoint> points,
        Ref<SurfaceUV> s1,
        Ref<SurfaceUV> s2,
        bool isClosed)
    {
        if (isClosed)
        {
            points.push_back(points[0]);
            for (int i = 0; i < 4; i++)
                points[points.size() - 1].Coords[i] += std::floorf(points[points.size() - 2].Coords[i]);
        }

        auto intersectionCurve = Ref<IntersectionCurve>(new IntersectionCurve(name, points, s1, s2, isClosed));

        //TODO createTrimming textures;

        return intersectionCurve;
    }
}
