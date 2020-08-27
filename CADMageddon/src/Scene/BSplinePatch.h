#pragma once
#include "cadpch.h"
#include "Point.h"
#include <glm/glm.hpp>

namespace CADMageddon
{
    class BSplinePatch
    {
    public:
        BSplinePatch(std::string name, int patchCountX, int patchCountY, int uDivisionCount = 4, int vDivisionCount = 4);

        std::vector<Ref<Point>> GetControlPoints() const { return m_ControlPoints; }
        std::vector<uint32_t> GetRenderingIndices() const { return m_Indices; }
        std::vector<uint32_t> GetGridIndices() const { return m_GridIndices; }

        bool GetShowPolygon() const { return m_ShowPolygon; }
        void SetShowPolygon(bool showPolygon) { m_ShowPolygon = showPolygon; }

        bool GetIsSelected() const { return m_IsSelected; }
        void SetIsSelected(bool isSelected) { m_IsSelected = isSelected; }

        std::string GetName() const { return m_Name; }
        void SetName(std::string name) { m_Name = name; }

        int GetUDivisionCount() const { return m_UDivisionCount; }
        void SetUDivisionCount(int uDivisionCount) { m_UDivisionCount = uDivisionCount; }

        int GetVDivisionCount() const { return m_VDivisionCount; }
        void SetVDivisionCount(int vDivisionCount) { m_VDivisionCount = vDivisionCount; }

        int GetPatchCountX() const { return m_PatchCountX; }
        int GetPatchCountY() const { return m_PatchCountY; }

        bool GetIsCylinder() const { return m_IsCylinder; }

        bool GetShowPoints() const { return m_ShowPoints; }
        void SetShowPoints(bool setShowPoints);

        static Ref<BSplinePatch> CreateRectPatch(
            std::string name,
            glm::vec3 startPosition,
            int PatchCountx,
            int PatchCounty,
            float width,
            float height,
            int uDivisionCount = 4,
            int vDivisionCount = 4);

        static Ref<BSplinePatch> CreateCyliderPatch(
            std::string name,
            glm::vec3 center,
            int PatchCountx,
            int PatchCounty,
            float radius,
            float height,
            int uDivisionCount = 4,
            int vDivisionCount = 4);

        //Used for deserialization
        static Ref<BSplinePatch> CreateBSplinePatch(
            std::string name,
            std::vector<Ref<Point>> controlPoints,
            int rowCount,
            int columnCount,
            int uDivisionCount,
            int vDivisionCount,
            bool isCylinder,
            bool showPolygon = false);

    private:
        void GenerateRectControlPoints(glm::vec3 startPosition, int PatchCountx, int PatchCounty, float width, float height);
        void GenerateCylinderControlPoints(glm::vec3 center, int PatchCountx, int PatchCounty, float radius, float height);

        void GenerateRectIndices(int PatchCountx, int PatchCounty);
        void GenerateCylinderIndices(int PatchCountx, int PatchCounty);

        void GenerateGridIndices(int rowCount, int columnCount);


    private:
        bool m_IsCylinder = false;
        bool m_ShowPoints = true;
        int m_PatchCountX;
        int m_PatchCountY;
        int m_UDivisionCount;
        int m_VDivisionCount;

        std::string m_Name;
        bool m_IsSelected = false;
        bool m_ShowPolygon = false;
        std::vector<Ref<Point>> m_ControlPoints;
        std::vector<uint32_t> m_Indices;
        std::vector<uint32_t> m_GridIndices;
    };
}