#pragma once
#include "cadpch.h"
#include "Point.h"
#include <glm/glm.hpp>
#include "BaseObject.h"
#include "SurfaceUV.h"

namespace CADMageddon
{
    class BezierPatch : public BaseObject, public SurfaceUV
    {
    public:
        BezierPatch(std::string name, int patchCountX, int patchCountY, int uDivisionCount = 4, int vDivisionCount = 4);

        std::vector<uint32_t> GetRenderingIndices() const { return m_Indices; }
        std::vector<uint32_t> GetGridIndices() const { return m_GridIndices; }

        bool GetShowPolygon() const { return m_ShowPolygon; }
        void SetShowPolygon(bool showPolygon) { m_ShowPolygon = showPolygon; }

        int GetUDivisionCount() const { return m_UDivisionCount; }
        void SetUDivisionCount(int uDivisionCount) { m_UDivisionCount = uDivisionCount; }

        int GetVDivisionCount() const { return m_VDivisionCount; }
        void SetVDivisionCount(int vDivisionCount) { m_VDivisionCount = vDivisionCount; }

        int GetPatchCountX() const { return m_PatchCountX; }
        int GetPatchCountY() const { return m_PatchCountY; }

        bool GetIsCylinder() const { return m_IsCylinder; }

        bool GetShowPoints() const { return m_ShowPoints; }
        void SetShowPoints(bool setShowPoints);

        static Ref<BezierPatch> CreateRectPatch(
            std::string name,
            glm::vec3 startPosition,
            int PatchCountx,
            int PatchCounty,
            float width,
            float height,
            int uDivisionCount = 4,
            int vDivisionCount = 4);

        static Ref<BezierPatch> CreateCyliderPatch(
            std::string name,
            glm::vec3 center,
            int PatchCountx,
            int PatchCounty,
            float radius,
            float height,
            int uDivisionCount = 4,
            int vDivisionCount = 4);

        //used for deserialization
        static Ref<BezierPatch> BezierPatch::CreateBezierPatch(
            std::string name,
            std::vector<Ref<Point>> controlPoints,
            int rowCount,
            int columnCount,
            int uDivisionCount,
            int vDivisionCount,
            bool isCylinder,
            bool showPolygon);

        virtual glm::vec3 GetPointAt(float u, float v) override;
        virtual glm::vec3 GetTangentUAt(float u, float v) override;
        virtual glm::vec3 GetTangentVAt(float u, float v) override;
        virtual float GetMinU() const override;
        virtual float GetMaxU() const override;
        virtual float GetMinV() const override;
        virtual float GetMaxV() const override;
        virtual bool GetRollU() const { return m_IsCylinder; }


    private:
        std::vector<uint32_t> GetPatchIndices(float u, float v);
        glm::vec4 BernsteinBasis(float t);
        glm::vec4 dBernsteinBasis(float t);
        void GenerateRectControlPoints(glm::vec3 startPosition, int PatchCountx, int PatchCounty, float width, float height);
        void GenerateCylinderControlPoints(glm::vec3 center, int PatchCountx, int PatchCounty, float radius, float height);

        void GenerateRectIndices(int PatchCountx, int PatchCounty);
        void GenerateCylinderIndices(int PatchCountx, int PatchCounty);

        void GenerateGridIndices(int rowCount, int columnCount);

    private:
        int m_PatchCountX;
        int m_PatchCountY;
        int m_UDivisionCount;
        int m_VDivisionCount;

        bool m_ShowPoints = true;

        bool m_IsCylinder = false;
        bool m_ShowPolygon = false;
        std::vector<uint32_t> m_Indices;
        std::vector<uint32_t> m_GridIndices;
    };
}