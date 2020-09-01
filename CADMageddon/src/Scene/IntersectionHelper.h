#pragma once
#include "Scene\IntersectionCurve.h"

namespace CADMageddon
{
    class IntersectionHelper
    {
    public:
        static std::vector<IntersectionPoint> GetIntersectionPoints(Ref<SurfaceUV> s1, Ref<SurfaceUV> s2, float stepSize, bool& isClosed);

    private:
        static glm::vec4 GetFirstPointFromTwoSurfaces(
            Ref<SurfaceUV> s1,
            Ref<SurfaceUV> s2,
            float divide = 5.0f
        );

        static IntersectionPoint GetNextIntersectionPoint(
            glm::vec4 parameters,
            std::vector<IntersectionPoint>& points,
            Ref<SurfaceUV> s1,
            Ref<SurfaceUV> s2,
            float stepSize,
            bool& found);

        static glm::vec4 GradientMinimalization(
            float uStart1,
            float vStart1,
            Ref<SurfaceUV> s1,
            float uStart2,
            float vStart2,
            Ref<SurfaceUV> s2);

        static float GetDistance(float uStart1,
            float vStart1,
            Ref<SurfaceUV> s1,
            float uStart2,
            float vStart2,
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
            glm::vec4 parameters,
            glm::vec4 minValues,
            glm::vec4 maxValues);

        static glm::bvec4 AreParametersInRange(glm::vec4 parameters, Ref<SurfaceUV> s1, Ref<SurfaceUV> s2);
       
    };
}