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
        glGenTextures(2, m_TextureIds);

        static const int TEX_SIZE = 8192;
        static unsigned char textureData[TEX_SIZE][TEX_SIZE];
        for (int k = 0; k < 2; k++)
        {
            bool isIn = false;
            std::vector<glm::vec2> surfaceCoordinates;
            if (k == 0)
                std::transform(m_IntersectionPoints.begin(), m_IntersectionPoints.end(), std::back_inserter(surfaceCoordinates), [](IntersectionPoint point) {return glm::vec2(point.Coords.x, point.Coords.y); });
            else
                std::transform(m_IntersectionPoints.begin(), m_IntersectionPoints.end(), std::back_inserter(surfaceCoordinates), [](IntersectionPoint point) {return glm::vec2(point.Coords.z, point.Coords.w); });

            float delta = 1.0f / (TEX_SIZE - 1);
            for (int i = 0; i < TEX_SIZE; i++)
            {
                int intersectionIndex = 0;
                float lineV = i * delta;
                auto intersections = GetIntersectionsAlongU(lineV, surfaceCoordinates);

                for (int j = 0; j < TEX_SIZE; j++)
                {
                    float lineU = (j + 1) / (float)(TEX_SIZE + 2) + 0.5f / TEX_SIZE;
                    CheckIfPixelInsideIntersections(lineU, intersections, intersectionIndex, isIn);

                    textureData[i][j] = isIn ? 255 : 0;
                }
            }

            glBindTexture(GL_TEXTURE_2D, m_TextureIds[k]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, TEX_SIZE, TEX_SIZE, 0, GL_RED, GL_UNSIGNED_BYTE, textureData);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
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
        m_IsClosed(isClosed),
        m_ShowPlot(false)
    {
        if (isClosed)
            GenerateTextures();
    }

    std::vector<float> IntersectionCurve::GetIntersectionsAlongU(float lineV, std::vector<glm::vec2> coords)
    {
        float offsets[] = { -1,0,1 };
        std::vector<float> intersections;
        float intersection;

        for (int i = 1; i < coords.size(); i++)
        {
            auto p1 = coords[i - 1];
            auto p2 = coords[i];

            GetIzolineIntersection(lineV, p1, p2, intersections);
            GetIzolineIntersection(lineV + 1, p1, p2, intersections);
            GetIzolineIntersection(lineV - 1, p1, p2, intersections);
        }

        std::sort(intersections.begin(), intersections.end());

        return intersections;
    }

    void IntersectionCurve::CheckIfPixelInsideIntersections(float pixel, std::vector<float> intersections, int& intersectionIndex, bool& isIn)
    {
        for (int i = 1; i < intersections.size(); i += 2)
        {
            auto intersectionLeft = intersections[i - 1];
            auto intersectionRight = intersections[i];
            
            if (intersectionLeft <= pixel && pixel <= intersectionRight)
            {
                isIn = true;
                return;
            }

            if ((intersectionLeft - 1) <= pixel && pixel <= (intersectionRight - 1))
            {
                isIn = true;
                return;
            }

            if ((intersectionLeft + 1) <= pixel && pixel <= (intersectionRight + 1))
            {
                isIn = true;
                return;
            }
        }

        isIn = false;
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

        if ((p1.y - line) * (p2.y - line) <= 0) //maybe <=
        {
            auto distance = p2 - p1;
            float x = p1.x + distance.x * (line - p1.y) / distance.y;
            intersections.push_back(x);
            return true;
        }

        return false;
    }

    std::vector<glm::vec2> IntersectionCurve::TriangulateCurve()
    {
        return std::vector<glm::vec2>();
    }


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
        }

        auto intersectionCurve = Ref<IntersectionCurve>(new IntersectionCurve(name, points, s1, s2, isClosed));

        return intersectionCurve;
    }
}
