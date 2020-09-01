#pragma once
#include <glm\glm.hpp>
#include "IntersectionCurve.h"

namespace CADMageddon
{
    class SurfaceUV
    {
    public:
        virtual glm::vec3 GetPointAt(float u, float v) = 0;
        virtual glm::vec3 GetTangentUAt(float u, float v) = 0;
        virtual glm::vec3 GetTangentVAt(float u, float v) = 0;

        virtual float GetMinU() const = 0;
        virtual float GetMaxU() const = 0;
        virtual float GetMinV() const = 0;
        virtual float GetMaxV() const = 0;

        Ref<IntersectionCurve> GetIntersectionCurve() const { return m_IntersectionCurve; }
        void SetIntersectionCurve(Ref<IntersectionCurve> intersectionCurve) { m_IntersectionCurve = intersectionCurve; }

        bool GetIsTrimmed() const { return m_IntersectionCurve != nullptr; }
        bool GetReverseTrimming() const { return m_ReverseTrimming; }
        void SetReverseTrimming(bool reverse) { m_ReverseTrimming = reverse; }

    private:
        bool m_ReverseTrimming;
        Ref<IntersectionCurve> m_IntersectionCurve = nullptr;

    };
}