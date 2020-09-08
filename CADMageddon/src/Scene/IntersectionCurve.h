#pragma once
#include "cadpch.h"
#include "InterpolatedCurve.h"
#include <glm\glm.hpp>
#include "Rendering\FrameBuffer.h"
#include "Rendering\Shader.h"

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
            std::vector<IntersectionPoint> points,
            Ref<SurfaceUV> s1,
            Ref<SurfaceUV> s2,
            IntersectionType intersectionType);

        std::vector<IntersectionPoint> GetIntersectionPoints() const { return m_IntersectionPoints; }
        Ref<InterpolatedCurve> ConvertToInterpolated(std::string name);

        Ref<SurfaceUV> GetFirstSurface() { return m_FirstSurface; }
        Ref<SurfaceUV> GetSecondSurface() { return m_SecondSurface; }

        bool GetShowPlot() const { return m_ShowPlot; }
        void SetShowPlot(bool showPlot) { m_ShowPlot = showPlot; }

        IntersectionType GetIntersectionType() const { return m_IntersectionType; }

        unsigned int GetFirstTextureId() const { return m_TextureIds[0]; }
        unsigned int GetSecondTextureId() const { return m_TextureIds[1]; }

        std::vector<glm::vec2> GetFirstSurfaceIntersectionLines() const { return m_TexturePixels[0]; }
        std::vector<glm::vec2> GetSecondSurfaceIntersectionLines() const { return m_TexturePixels[1]; }

        void CalculateTrimming(int lineCount, bool isFirst);

    private:
        bool m_ShowPlot;

        void GenerateTexture(int texNumber);

        IntersectionCurve(
            std::string name,
            std::vector<IntersectionPoint> points,
            Ref<SurfaceUV> firstSurface,
            Ref<SurfaceUV> secondSurface,
            IntersectionType intersectionType);

        void GetIntersectionsAlongU(float lineV, std::vector<glm::vec2> coords, std::vector<float>& intersections);
        void GetIntersectionsAlongV(float lineU, std::vector<glm::vec2> coords, std::vector<float>& intersections);
        bool CheckIfPixelInsideIntersections(float pixel, std::vector<float> intersections);
        bool GetIzolineIntersection(float line, glm::vec2 p1, glm::vec2 p2, std::vector<float>& intersection);

        std::vector<IntersectionPoint> m_IntersectionPoints;
        Ref<SurfaceUV> m_FirstSurface;
        Ref<SurfaceUV> m_SecondSurface;

        IntersectionType m_IntersectionType;
        std::vector<glm::vec2> m_TexturePixels[2];

        unsigned int m_TextureIds[2];
    };
}