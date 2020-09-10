#pragma once
#include "Scene\IntersectionCurve.h"
#include "CADApplication\Systems\Cursor3D.h"

namespace CADMageddon
{
    class IntersectionHelper
    {
    public:
        static std::vector<std::vector<glm::vec2>>* GetIntersectionPoints(
            Ref<Cursor3D> cursor, 
            Ref<SurfaceUV> s1,
            Ref<SurfaceUV> s2, 
            float stepSize, 
            IntersectionType& intersectionType,
            std::vector<IntersectionPoint>& intersectionPoints);

        static  std::vector<std::vector<glm::vec2>>* GetIntersectionPoints(
            Ref<SurfaceUV> s1,
            Ref<SurfaceUV> s2, 
            float stepSize, 
            IntersectionType& intersectionType,
            std::vector<IntersectionPoint>& intersectionPoints);

    private:

        static  std::vector<std::vector<glm::vec2>>* GetIntersectionPoints(
            glm::vec4 firstPoint,
            Ref<SurfaceUV> s1,
            Ref<SurfaceUV> s2,
            float stepSize,
            IntersectionType& intersectionType,
            std::vector<IntersectionPoint>& intersectionPoints);

        static void MergeLastAndFirstLoop(std::vector<std::vector<glm::vec2>>& loop);

        static void GetLastTwoPoints(
            Ref<SurfaceUV> s1,
            Ref<SurfaceUV> s2,
            IntersectionPoint& last,
            std::vector<IntersectionPoint>& intersectionPoints,
            std::vector<std::vector<glm::vec2>>& firstLoops,
            std::vector<std::vector<glm::vec2>>& secondLoops);

        static glm::vec4 GetFirstPointFromOneSurfaceCursor(Ref<SurfaceUV> s1, Ref<Cursor3D> cursor, float divide = 5.0f);

        static glm::vec4 GetFirstPointFromOneSurface(Ref<SurfaceUV> s1, float divide = 5.0f);

        static glm::vec4 GetFirstPointFromTwoSurfacesCursor(Ref<Cursor3D> cursor, Ref<SurfaceUV> s1, Ref<SurfaceUV> s2, float divide = 5.0f);

        static glm::vec4 GetFirstPointFromTwoSurfaces(
            Ref<SurfaceUV> s1,
            Ref<SurfaceUV> s2,
            float divide = 5.0f
        );

        static IntersectionPoint GetNextIntersectionPoint(
            glm::vec4 parameters,
            Ref<SurfaceUV> s1,
            Ref<SurfaceUV> s2,
            float stepSize,
            bool reversed);

        static glm::vec4 GradientMinimalization(
            glm::vec4 parameters,
            Ref<SurfaceUV> s1,
            Ref<SurfaceUV> s2);

        static float GetDistance(
            glm::vec4 parameters,
            Ref<SurfaceUV> s1,
            Ref<SurfaceUV> s2);

        static glm::vec4 GetNegativeGradient(
            float uStart1,
            float vStart1,
            Ref<SurfaceUV> s1,
            float uStart2,
            float vStart2,
            Ref<SurfaceUV> s2);

        static float GoldenRatioSearch(
            float alfaMin,
            float alfaMax,
            glm::vec4 x,
            glm::vec4 direction,
            Ref<SurfaceUV> s1,
            Ref<SurfaceUV> s2,
            float tolerance = 1e-5);

        static glm::vec4 ClampParameters(
            glm::vec4& parameters,
            Ref<SurfaceUV> s1,
            Ref<SurfaceUV> s2);

        static bool CheckParameters(glm::vec2& parameters, Ref<SurfaceUV> surface, std::vector<std::vector<glm::vec2>>& loops);

        static IntersectionType GetIntersectionType(IntersectionType intersectionType, glm::vec4& parameters, Ref<SurfaceUV> s1, Ref<SurfaceUV> s2);
    };
}