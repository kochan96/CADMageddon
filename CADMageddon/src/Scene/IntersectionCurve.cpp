#include "IntersectionCurve.h"
#include "LineClipper.h"
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
        /* int index = isFirst ? 0 : 1;

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
         }*/
    }

    void IntersectionCurve::GenerateTextures(int texNumber)
    {
        GLfloat bkColor[4];
        glGetFloatv(GL_COLOR_CLEAR_VALUE, bkColor);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        std::vector<std::vector<glm::vec2>> boundary = m_Boundary[texNumber];
        GenerateTrimTexture(m_Boundary[texNumber], m_TrimInsideWithBoundary[texNumber]);
        GenerateTrimTexture(m_DomainLoops[texNumber], m_TrimInsideTexture[texNumber]);

        glClearColor(bkColor[0], bkColor[1], bkColor[2], bkColor[3]);
    }

    void IntersectionCurve::GenerateTrimTexture(std::vector<std::vector<glm::vec2>> points, unsigned int& texNumber)
    {
        const int TEX_SIZE = 8192;
        unsigned int renderId;
        glGenFramebuffers(1, &renderId);
        glBindFramebuffer(GL_FRAMEBUFFER, renderId);

        glGenTextures(1, &texNumber);
        glBindTexture(GL_TEXTURE_2D, texNumber);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, TEX_SIZE, TEX_SIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texNumber, 0);

        unsigned int depthAttachment;
        glGenTextures(1, &depthAttachment);
        glBindTexture(GL_TEXTURE_2D, depthAttachment);
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, TEX_SIZE, TEX_SIZE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthAttachment, 0);

        glViewport(0, 0, TEX_SIZE, TEX_SIZE);
        glEnable(GL_STENCIL_TEST);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        RenderTrimTexture(points);

        glDisable(GL_STENCIL_TEST);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glDeleteFramebuffers(1, &renderId);
        glDeleteTextures(1, &depthAttachment);
    }

    void IntersectionCurve::RenderTrimTexture(std::vector<std::vector<glm::vec2>> points)
    {
        size_t maxSize = 0;
        for (auto p : points)
        {
            if (maxSize < p.size())
                maxSize = p.size();
        }
        auto m_VertexArray = CreateScope<OpenGLVertexArray>();
        auto m_VertexBuffer = CreateRef<OpenGLVertexBuffer>(maxSize * 2 * sizeof(float));
        m_VertexBuffer->SetLayout({
                {ShaderDataType::Float2,"a_Position"}
            });
        m_VertexArray->AddVertexBuffer(m_VertexBuffer);

        for (int i = 0; i < points.size(); i++)
        {
            if (points[i].empty())
                continue;

            m_VertexBuffer->SetData(&points[i].data()[0].x, points[i].size() * 2 * sizeof(float));

            glClear(GL_STENCIL_BUFFER_BIT);
            glStencilFunc(GL_ALWAYS, 0, 1);
            glStencilOp(GL_INVERT, GL_INVERT, GL_INVERT);
            glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
            glDrawArrays(GL_TRIANGLE_FAN, 0, points[i].size());

            glClear(GL_DEPTH_BUFFER_BIT);
            glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
            glStencilFunc(GL_EQUAL, 1, 1);
            glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
            glDrawArrays(GL_TRIANGLE_FAN, 0, points[i].size());
        }
    }

    IntersectionCurve::IntersectionCurve(
        std::string name,
        std::vector<std::vector<glm::vec2>>* points,
        Ref<SurfaceUV> s1,
        Ref<SurfaceUV> s2,
        IntersectionType intersectionType,
        std::vector<IntersectionPoint> intersectionPoints) :
        BaseObject(name),
        m_FirstSurface(s1),
        m_SecondSurface(s2),
        m_IntersectionPoints(intersectionPoints),
        m_ShowPlot(false)
    {
        const int TEX_SIZE = 8192;


        m_DomainLoops[0] = points[0];
        m_DomainLoops[1] = points[1];

        for (auto domainLoop : m_DomainLoops)
        {
            for (auto loop : domainLoop)
            {
                if (!loop.empty())
                    loop.push_back(loop.front());
            }
        }

        m_Shader = CreateRef<OpenGLShader>("assets/shaders/TrimTextureShader.glsl");
        m_Shader->Bind();

        if (intersectionType == IntersectionType::ClosedClosed || intersectionType == IntersectionType::ClosedOpen)
        {
            m_Boundary[0] = ConvertToClosedLoops(points[0]);
            FramebufferSpecification fbSpec;
            fbSpec.Width = TEX_SIZE;
            fbSpec.Height = TEX_SIZE;
            m_FrameBuffer[0] = CreateRef<OpenGLFramebuffer>(fbSpec);

            GenerateTextures(0);
        }

        if (intersectionType == IntersectionType::ClosedClosed || intersectionType == IntersectionType::OpenClosed)
        {
            m_Boundary[1] = ConvertToClosedLoops(points[1]);
            FramebufferSpecification fbSpec;
            fbSpec.Width = TEX_SIZE;
            fbSpec.Height = TEX_SIZE;
            m_FrameBuffer[1] = CreateScope<OpenGLFramebuffer>(fbSpec);

            GenerateTextures(1);
        }

        m_Shader->Unbind();

        m_IntersectionType = intersectionType;
    }

    void IntersectionCurve::GetIntersectionsAlongU(int lineV, std::vector<glm::ivec2> coords, std::vector<int>& intersections)
    {
        for (int j = 1; j < coords.size(); j++)
        {
            auto p1 = coords[j - 1];
            auto p2 = coords[j];

            GetIzolineIntersection(lineV, p1, p2, intersections);
        }

        std::sort(intersections.begin(), intersections.end());
    }

    void IntersectionCurve::GetIntersectionsAlongV(int lineU, std::vector<std::vector<glm::ivec2>> coords, std::vector<int>& intersections)
    {
        for (int i = 0; i < coords.size(); i++)
        {
            for (int j = 1; j < coords[i].size(); j++)
            {
                auto p1 = coords[i][j - 1];
                auto p2 = coords[i][j];

                p1 = glm::vec2(p1.y, p1.x);
                p2 = glm::vec2(p2.y, p2.x);

                GetIzolineIntersection(lineU, p1, p2, intersections);
            }
        }

        std::sort(intersections.begin(), intersections.end());
    }


    bool IntersectionCurve::GetIzolineIntersection(int line, glm::ivec2 p1, glm::ivec2 p2, std::vector<int>& intersections)
    {
        /*if (p1.y == line && p2.y == line)
        {
            return false;
        }

        if (p1.y == line)
        {
            intersections.push_back(p1.x);
            //intersections.push_back(p1.x);
            return true;
        }

        if (p2.y == line)
        {
            intersections.push_back(p2.x);
            //intersections.push_back(p2.x);
            return line;
        }*/

        if ((p1.y - line) * (p2.y - line) < 0) //maybe <=
        {
            auto distance = p2 - p1;
            int x = p1.x + distance.x * (line - p1.y) / distance.y;
            intersections.push_back(x);
            return true;
        }

        return false;
    }

    std::vector<std::vector<glm::vec2>> IntersectionCurve::ConvertToClosedLoops(const std::vector<std::vector<glm::vec2>>& loops)
    {
        glm::vec2 xBoundary(0.0f, 1.0f);
        glm::vec2 yBoundary(0.0f, 1.0f);
        bool intersects = false;
        for (auto loop : loops)
        {
            intersects = CheckIfIntersectWithBoundary(xBoundary, yBoundary, loop);
            if (intersects)
                break;
        }

        std::vector<std::vector<glm::vec2>> result;

        if (!intersects)
        {
            for (auto loop : loops)
            {
                if (!loop.empty())
                {
                    loop.push_back(loop.front());
                    result.push_back(loop);
                }
            }

            return result;
        }

        for (auto loop : loops)
        {
            if (!loop.empty())
                result.push_back(ConvertToClosed(xBoundary, yBoundary, loop));
        }

        return result;
    }

    std::vector<glm::vec2> IntersectionCurve::ConvertToClosed(glm::vec2 xBoundary, glm::vec2 yBoundary, std::vector<glm::vec2> loop)
    {
        glm::vec2 bottomLeft(xBoundary.x, yBoundary.x);
        glm::vec2 bottomRight(xBoundary.y, yBoundary.x);
        glm::vec2 topRight(xBoundary.y, yBoundary.y);
        glm::vec2 topLeft(xBoundary.x, yBoundary.y);
        auto loopFront = loop.front();


        auto boundary = GetBoundaryConnectionType(xBoundary, yBoundary, loop);

        switch (boundary)
        {
            case BoundaryConnectionType::LEFTRIGHT:
            {
                loop.push_back(bottomRight);
                loop.push_back(bottomLeft);
                loop.push_back(loopFront);
                break;
            }
            case BoundaryConnectionType::TOPLEFT:
            case BoundaryConnectionType::LEFTTOP:
            {
                loop.push_back(topLeft);
                loop.push_back(loopFront);
                break;
            }
            case BoundaryConnectionType::BOTTOMLEFT:
            case BoundaryConnectionType::LEFTBOTTOM:
            {
                loop.push_back(bottomLeft);
                loop.push_back(loopFront);
                break;
            }
            case BoundaryConnectionType::RIGHTLEFT:
            {
                loop.push_back(topLeft);
                loop.push_back(topRight);
                loop.push_back(loopFront);
                break;
            }
            case BoundaryConnectionType::TOPRIGHT:
            case BoundaryConnectionType::RIGHTTOP:
            {
                loop.push_back(topRight);
                loop.push_back(loopFront);
                break;
            }
            case BoundaryConnectionType::BOTTOMRIGHT:
            case BoundaryConnectionType::RIGHTBOTTOM:
            {
                loop.push_back(bottomRight);
                loop.push_back(loopFront);
                break;
            }
            case BoundaryConnectionType::BOTTOMTOP:
            {
                loop.push_back(topRight);
                loop.push_back(bottomRight);
                loop.push_back(loopFront);
                break;
            }
            case BoundaryConnectionType::TOPBOTTOM:
            {
                loop.push_back(bottomLeft);
                loop.push_back(topLeft);
                loop.push_back(loopFront);
                break;
            }
            case BoundaryConnectionType::LEFTLEFT:
            case BoundaryConnectionType::RIGHTRIGHT:
            case BoundaryConnectionType::TOPTOP:
            case BoundaryConnectionType::BOTTOMBOTTOM:
            {
                loop.push_back(loopFront);
                break;
            }
        }

        return loop;
    }

    bool IntersectionCurve::CheckIfIntersectWithBoundary(glm::vec2 xBoundary, glm::vec2 yBoundary, const std::vector<glm::vec2>& loop)
    {
        if (loop.empty())
            return false;

        const float xmin = xBoundary.x;
        const float xmax = xBoundary.y;
        const float ymin = yBoundary.x;
        const float ymax = yBoundary.y;

        if (loop.front().x == xmin || loop.back().x == xmin)
            return true;

        if (loop.front().x == xmax || loop.back().x == xmax)
            return true;

        if (loop.front().y == ymin || loop.back().y == ymin)
            return true;

        if (loop.front().x == ymax || loop.back().y == ymax)
            return true;

        return false;
    }

    IntersectionCurve::BoundaryConnectionType IntersectionCurve::GetBoundaryConnectionType(glm::vec2 xBoundary, glm::vec2 yBoundary, const std::vector<glm::vec2>& loop)
    {
        if (loop.empty())
            return BoundaryConnectionType::INVALID;

        const float xmin = xBoundary.x;
        const float xmax = xBoundary.y;
        const float ymin = yBoundary.x;
        const float ymax = yBoundary.y;

        if (loop.front().x == xmin)
        {
            if (loop.back().x == xmax)
                return BoundaryConnectionType::LEFTRIGHT;
            else if (loop.back().x == xmin)
                return BoundaryConnectionType::LEFTLEFT;
            else if (loop.back().y == ymin)
                return BoundaryConnectionType::LEFTBOTTOM;
            else if (loop.back().y == ymax)
                return BoundaryConnectionType::LEFTTOP;
        }
        else if (loop.front().x == xmax)
        {
            if (loop.back().x == xmin)
                return BoundaryConnectionType::RIGHTLEFT;
            else if (loop.back().x == xmax)
                return BoundaryConnectionType::RIGHTRIGHT;
            else if (loop.back().y == ymin)
                return BoundaryConnectionType::RIGHTBOTTOM;
            else if (loop.back().y == ymax)
                return BoundaryConnectionType::RIGHTTOP;
        }
        else if (loop.front().y == ymin)
        {
            if (loop.back().y == ymax)
                return BoundaryConnectionType::BOTTOMTOP;
            else if (loop.back().y == ymin)
                return BoundaryConnectionType::BOTTOMBOTTOM;
            else if (loop.back().x == xmin)
                return BoundaryConnectionType::BOTTOMLEFT;
            else if (loop.back().x == xmax)
                return BoundaryConnectionType::BOTTOMRIGHT;
        }
        else if (loop.front().y == ymax)
        {
            if (loop.back().y == ymin)
                return BoundaryConnectionType::TOPBOTTOM;
            else if (loop.back().y == ymax)
                return BoundaryConnectionType::TOPTOP;
            else if (loop.back().x == xmin)
                return BoundaryConnectionType::TOPLEFT;
            else if (loop.back().x == xmax)
                return BoundaryConnectionType::TOPRIGHT;
        }

        return BoundaryConnectionType::INVALID;
    }

    Ref<IntersectionCurve> IntersectionCurve::Create(
        std::string name,
        std::vector<std::vector<glm::vec2>>* points,
        Ref<SurfaceUV> s1,
        Ref<SurfaceUV> s2,
        IntersectionType intersectionType,
        std::vector<IntersectionPoint> intersectionPoints)
    {
        auto intersectionCurve = Ref<IntersectionCurve>(new IntersectionCurve(name, points, s1, s2, intersectionType, intersectionPoints));

        return intersectionCurve;
    }
}
