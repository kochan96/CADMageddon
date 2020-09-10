#pragma once
#include <glm\glm.hpp>
#include "IntersectionCurve.h"

namespace CADMageddon
{
    enum class TrimmingType
    {
        Inside,
        InsideWithBoundary,
    };

    class SurfaceUV : public std::enable_shared_from_this<SurfaceUV>
    {
    public:
        virtual glm::vec3 GetPointAt(float u, float v) = 0;
        virtual glm::vec3 GetTangentUAt(float u, float v) = 0;
        virtual glm::vec3 GetTangentVAt(float u, float v) = 0;

        virtual float GetMinU() const = 0;
        virtual float GetMaxU() const = 0;
        virtual float GetMinV() const = 0;
        virtual float GetMaxV() const = 0;

        virtual int GetUDivision() const = 0;
        virtual int GetVDivision() const = 0;

        Ref<IntersectionCurve> GetIntersectionCurve() const { return m_IntersectionCurve; }
        void SetIntersectionCurve(Ref<IntersectionCurve> intersectionCurve) { m_IntersectionCurve = intersectionCurve; RecalculateTrimCurveGrid(); }

        bool GetIsTrimmed() const { return m_IntersectionCurve != nullptr; }
        bool GetReverseTrimming() const { return m_ReverseTrimming; }
        void SetReverseTrimming(bool reverse) { m_ReverseTrimming = reverse; }

        TrimmingType GetTrimmingType() const { return m_TrimmingType; }
        void SetTrimmingType(TrimmingType trimmingType) { m_TrimmingType = trimmingType; }


        unsigned int GetTextureId() const
        {
            if (m_IntersectionCurve)
            {
                if (m_IntersectionCurve->GetFirstSurface() == shared_from_this()) //hacky
                {
                    if (m_TrimmingType == TrimmingType::InsideWithBoundary)
                        return m_IntersectionCurve->GetFirstSurfaceTrimmedWithBounds();

                    return m_IntersectionCurve->GetFirstSurfaceTrimmedInside();
                }
                else
                {
                    if (m_TrimmingType == TrimmingType::InsideWithBoundary)
                        return m_IntersectionCurve->GetSecondSurfaceTrimmedWithBounds();

                    return m_IntersectionCurve->GetSecondSurfaceTrimmedInside();
                }
            }

            return -1;
        }

        virtual bool GetRollU() const { return false; }
        virtual bool GetRollV() const { return false; }

    protected:
        void RecalculateTrimCurveGrid()
        {
            /*if (m_IntersectionCurve)
            {
                bool isFirst = m_IntersectionCurve->GetFirstSurface() == shared_from_this();
                m_IntersectionCurve->CalculateTrimming(GetVDivision(), isFirst);
            }*/
        }

    private:
        TrimmingType m_TrimmingType = TrimmingType::InsideWithBoundary;
        bool m_ReverseTrimming = false;
        Ref<IntersectionCurve> m_IntersectionCurve = nullptr;

    };
}