#pragma once
#include "cadpch.h"
#include "InterpolatedCurve.h"
#include <glm\glm.hpp>

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

    private:
        bool m_ShowPlot;

        void GenerateTextures();

        bool GetIzolineIntersection(float line, glm::vec2 p1, glm::vec2 p2, bool isReversed, float& intersection);

        std::vector<glm::vec2> CalculateTrimming(int lineCount, bool alongU, bool isFirst);

        IntersectionCurve(
            std::string name,
            std::vector<IntersectionPoint> points,
            Ref<SurfaceUV> firstSurface,
            Ref<SurfaceUV> secondSurface,
            bool isClosed);

        std::vector<IntersectionPoint> m_IntersectionPoints;
        Ref<SurfaceUV> m_FirstSurface;
        Ref<SurfaceUV> m_SecondSurface;

        bool m_IsClosed;

        unsigned int m_TexrureIds[2];
    };
}