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

    class IntersectionCurve : public BaseObject
    {
    public:
        static Ref<IntersectionCurve> Create(
            std::string name,
            std::vector<IntersectionPoint> points,
            Ref<SurfaceUV> s1,
            Ref<SurfaceUV> s2,
            bool isClosed);

        std::vector<IntersectionPoint> GetIntersectionPoints() const { return m_IntersectionPoints; }
        Ref<InterpolatedCurve> ConvertToInterpolated(std::string name);

        Ref<SurfaceUV> GetFirstSurface() { return m_FirstSurface; }
        Ref<SurfaceUV> GetSecondSurface() { return m_SecondSurface; }

        bool GetShowPlot() const { return m_ShowPlot; }
        void SetShowPlot(bool showPlot) { m_ShowPlot = showPlot; }

        bool GetIsClosed() const { return m_IsClosed; }

        unsigned int GetFirstTextureId() const { return m_TextureIds[0]; }
        unsigned int GetSecondTextureId() const { return m_TextureIds[1]; }

    private:
        bool m_ShowPlot;

        void GenerateTextures();

        IntersectionCurve(
            std::string name,
            std::vector<IntersectionPoint> points,
            Ref<SurfaceUV> firstSurface,
            Ref<SurfaceUV> secondSurface,
            bool isClosed);

        std::vector<float> GetIntersectionsAlongU(float lineV, std::vector<glm::vec2> coords);
        void CheckIfPixelInsideIntersections(float pixel, std::vector<float> intersections, int& intersectionIndex, bool& isIn);
        bool GetIzolineIntersection(float line, glm::vec2 p1, glm::vec2 p2, std::vector<float>& intersection);

        std::vector<IntersectionPoint> m_IntersectionPoints;
        Ref<SurfaceUV> m_FirstSurface;
        Ref<SurfaceUV> m_SecondSurface;

        bool m_IsClosed;

        std::vector<glm::vec2> TriangulateCurve();

        unsigned int m_TextureIds[2];
    };
}