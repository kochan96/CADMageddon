#pragma once
#include "cadpch.h"
#include "InterpolatedCurve.h"
#include <glm\glm.hpp>
#include "Rendering\FrameBuffer.h"
#include "Rendering\Shader.h"
#include "Rendering\VertexArray.h"

namespace CADMageddon
{
    class SurfaceUV;

    struct IntersectionPoint
    {
        glm::vec4 Coords;
        glm::vec3 Location;
    };

    enum class IntersectionType
    {
        OpenOpen = 0,
        ClosedOpen = 1,
        OpenClosed = 2,
        ClosedClosed = 3
    };

    class IntersectionCurve : public BaseObject
    {
    public:
        static Ref<IntersectionCurve> Create(
            std::string name,
            std::vector<std::vector<glm::vec2>>* points,
            Ref<SurfaceUV> s1,
            Ref<SurfaceUV> s2,
            IntersectionType intersectionType,
            std::vector<IntersectionPoint> intersectionPoints);

        std::vector<IntersectionPoint> GetIntersectionPoints() const { return m_IntersectionPoints; }
        Ref<InterpolatedCurve> ConvertToInterpolated(std::string name);

        Ref<SurfaceUV> GetFirstSurface() { return m_FirstSurface; }
        Ref<SurfaceUV> GetSecondSurface() { return m_SecondSurface; }

        bool GetShowPlot() const { return m_ShowPlot; }
        void SetShowPlot(bool showPlot) { m_ShowPlot = showPlot; }

        IntersectionType GetIntersectionType() const { return m_IntersectionType; }

        unsigned int GetFirstSurfaceTrimmedInside() const { return m_TrimInsideTexture[0]; }
        unsigned int GetSecondSurfaceTrimmedInside() const { return m_TrimInsideTexture[1]; }

        unsigned int GetFirstSurfaceTrimmedWithBounds() const { return m_TrimInsideWithBoundary[0]; }
        unsigned int GetSecondSurfaceTrimmedWithBounds() const { return m_TrimInsideWithBoundary[1]; }


        std::vector<std::vector<glm::vec2>> GetFirstSurfaceDomainLoops() const { return m_DomainLoops[0]; }
        std::vector<std::vector<glm::vec2>> GetSecondSurfaceDomainLoops() const { return m_DomainLoops[1]; }

        std::vector<std::vector<glm::vec2>> GetFirstBoundary() const { return m_Boundary[0]; }
        std::vector<std::vector<glm::vec2>> GetSecondBoundary() const { return m_Boundary[1]; }


        /*std::vector<glm::vec2> GetFirstSurfaceIntersectionLines() const { return m_IntersectionLines[0]; }
        std::vector<glm::vec2> GetSecondSurfaceIntersectionLines() const { return m_IntersectionLines[1]; }*/

        void CalculateTrimming(int lineCount, bool isFirst);

    private:
        Ref<OpenGLFramebuffer> m_FrameBuffer[2];
        Ref<OpenGLShader> m_Shader;


        bool m_ShowPlot;

        void GenerateTextures(int texNumber);
        void GenerateTrimTexture(std::vector<std::vector<glm::vec2>> points, unsigned int& texNumber);

        void RenderTrimTexture(std::vector<std::vector<glm::vec2>> points);

        IntersectionCurve(
            std::string name,
            std::vector<std::vector<glm::vec2>>* points,
            Ref<SurfaceUV> firstSurface,
            Ref<SurfaceUV> secondSurface,
            IntersectionType intersectionType,
            std::vector<IntersectionPoint> intersectionPoints);

        void GetIntersectionsAlongU(int lineV, std::vector<glm::ivec2> coords, std::vector<int>& intersections);
        void GetIntersectionsAlongV(int lineU, std::vector<std::vector<glm::ivec2>> coords, std::vector<int>& intersections);
        bool GetIzolineIntersection(int line, glm::ivec2 p1, glm::ivec2 p2, std::vector<int>& intersection);

        enum class BoundaryConnectionType
        {
            LEFTLEFT,
            LEFTRIGHT,
            LEFTBOTTOM,
            LEFTTOP,

            RIGHTRIGHT,
            RIGHTLEFT,
            RIGHTBOTTOM,
            RIGHTTOP,

            BOTTOMBOTTOM,
            BOTTOMLEFT,
            BOTTOMTOP,
            BOTTOMRIGHT,

            TOPTOP,
            TOPLEFT,
            TOPBOTTOM,
            TOPRIGHT,

            INVALID,
        };

        std::vector<std::vector<glm::vec2>> ConvertToClosedLoops(const std::vector<std::vector<glm::vec2>>& loops);
        bool CheckIfIntersectWithBoundary(glm::vec2 xBoundary, glm::vec2 yBoundary, const std::vector<glm::vec2>& loop);
        BoundaryConnectionType GetBoundaryConnectionType(glm::vec2 xBoundary, glm::vec2 yBoundary, const std::vector<glm::vec2>& loop);

        std::vector<glm::vec2> ConvertToClosed(glm::vec2 xBoundary, glm::vec2 yBoundary, std::vector<glm::vec2> loop);
        std::vector<std::vector<glm::vec2>> m_DomainLoops[2];

        std::vector<std::vector<glm::vec2>> m_Boundary[2];

        std::vector<IntersectionPoint> m_IntersectionPoints;
        Ref<SurfaceUV> m_FirstSurface;
        Ref<SurfaceUV> m_SecondSurface;

        IntersectionType m_IntersectionType;
        std::vector<glm::vec2> m_IntersectionLines[2];

        unsigned int m_TrimInsideTexture[2];
        unsigned int m_TrimInsideWithBoundary[2];
    };
}