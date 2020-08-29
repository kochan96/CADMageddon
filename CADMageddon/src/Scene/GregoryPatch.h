#pragma once
#include "BezierPatch.h"

namespace CADMageddon
{
    enum class Border
    {
        B30, B012, B1215, B153, B03, B120, B1512, B315, None
    };

    enum class Fill
    {
        B12, B23, B31
    };

    struct BoundaryCurve
    {
        glm::vec3 Bottom; // middle patch point
        glm::vec3 Second;
        glm::vec3 Third;
        glm::vec3 Fourth; //central point
    };

    struct BoundaryCurves
    {
        BoundaryCurve p1; //b1 Curve
        BoundaryCurve p2; //b2 Curve
        BoundaryCurve p3; //b3 Curve

        glm::vec3 Center;
    };

    struct GFieldVectors
    {
        glm::vec3 a0;
        glm::vec3 a3;
        glm::vec3 b0;
        glm::vec3 b3;
        glm::vec3 g0;
        glm::vec3 g1;
        glm::vec3 g2;
    };

    struct CFieldVectors
    {
        glm::vec3 C0;
        glm::vec3 C05;
        glm::vec3 C1;

        glm::vec3 c0;
        glm::vec3 c1;
        glm::vec3 c2;
    };

    struct DFieldVectors
    {
        glm::vec3 d0;
        glm::vec3 d1;
        glm::vec3 d2;
        glm::vec3 d3;
    };

    struct GregoryPoints
    {
        glm::vec3 p0;
        glm::vec3 e0_m;
        glm::vec3 e0_p;
        glm::vec3 f0_m;
        glm::vec3 f0_p;

        glm::vec3 p1;
        glm::vec3 e1_m;
        glm::vec3 e1_p;
        glm::vec3 f1_m;
        glm::vec3 f1_p;

        glm::vec3 p2;
        glm::vec3 e2_m;
        glm::vec3 e2_p;
        glm::vec3 f2_m;
        glm::vec3 f2_p;

        glm::vec3 p3;
        glm::vec3 e3_m;
        glm::vec3 e3_p;
        glm::vec3 f3_m;
        glm::vec3 f3_p;
    };

    struct FillingData
    {
        BoundaryCurve boundaryCurvesPoints;
        GFieldVectors gFieldVectors;
        CFieldVectors cFieldVectors;
        DFieldVectors dFieldVectoes;

        BoundaryCurve boundaryCurvesPointsLeft;
        CFieldVectors cFieldVectorsLeft;
        GFieldVectors gFieldVectorsLeft;
        DFieldVectors dFieldVectorsLeft;

        GregoryPoints gregoryPoints;

        bool ShowMesh = false;
    };

    struct GregoryCorner {
        glm::vec3 p, e[2], f[2];
    };

    class GregoryPatch : public BaseObject
    {
    public:
        static Ref<GregoryPatch> Create(
            Ref<BezierPatch> b1,
            Ref<BezierPatch> b2,
            Ref<BezierPatch> b3,
            Ref<Point> commonPoints[3]);

        FillingData GetFillingData(Fill fill) const;

        int GetUDivisionCount() const { return m_UDivisionCount; }
        void SetUDivisionCount(int uDivisionCount) { m_UDivisionCount = uDivisionCount; }

        int GetVDivisionCount() const { return m_VDivisionCount; }
        void SetVDivisionCount(int vDivisionCount) { m_VDivisionCount = vDivisionCount; }

        Ref<BezierPatch> GetB1() const { return b1; }
        Ref<BezierPatch> GetB2() const { return b2; }
        Ref<BezierPatch> GetB3() const { return b3; }


        bool GetShowFirst() const { return m_ShowFirst; }
        void SetShowFirst(bool showFirst) { m_ShowFirst = showFirst; }
        bool GetShowFirstMesh() const { return m_ShowFirstMesh; }
        void SetShowFirstMesh(bool showFirst) { m_ShowFirstMesh = showFirst; }

        bool GetShowSecond() const { return m_ShowSecond; }
        void SetShowSecond(bool showSecond) { m_ShowSecond = showSecond; }
        bool GetShowSecondMesh() const { return m_ShowSecondMesh; }
        void SetShowSecondMesh(bool showSecond) { m_ShowSecondMesh = showSecond; }

        bool GetShowThird() const { return m_ShowThird; }
        void SetShowThird(bool showThird) { m_ShowThird = showThird; }
        bool GetShowThirdMesh() const { return m_ShowThirdMesh; }
        void SetShowThirdMesh(bool showThird) { m_ShowFirstMesh = showThird; }


    private:
        GregoryPatch(std::string name, Border border[3]);
        static std::vector<glm::vec3> GetSecondHalfBezier(std::vector<glm::vec3> curve);
        static std::vector<glm::vec3> GetFirstHalfBezier(std::vector<glm::vec3> curve);
        static glm::vec3 BezierValue(std::vector<glm::vec3> curve, float t);
        static glm::vec3 BezierFirstDerivativeValue(std::vector<glm::vec3> curve, float t);

        static CFieldVectors GetCField(BoundaryCurve curve);
        static GFieldVectors GetGField(glm::vec3 a0, glm::vec3 a3, glm::vec3 b0, glm::vec3 b3);
        static DFieldVectors GetDField(GFieldVectors gField, CFieldVectors cField);
        BoundaryCurves GetBoundaryCurvePoints() const;

        static Border GetBorderEnum(Ref<BezierPatch> b1, Ref<Point> p0, Ref<Point> p1);
        static std::vector<glm::vec3> GetBorderPoints(Ref<BezierPatch> b, Border border);
        FillingData GetFillingData(
            std::vector<glm::vec3> border,
            std::vector<glm::vec3> borderLeft,
            BoundaryCurve boundaryCurve,
            BoundaryCurve left,
            BoundaryCurve right,
            bool showMesh) const;

    private:
        int m_UDivisionCount = 4;
        int m_VDivisionCount = 4;

        bool m_ShowFirst = true;
        bool m_ShowSecond = true;
        bool m_ShowThird = true;

        bool m_ShowFirstMesh = true;
        bool m_ShowSecondMesh = true;
        bool m_ShowThirdMesh = true;

        Border m_Border[3];

        Ref<BezierPatch> b1;
        Ref<BezierPatch> b2;
        Ref<BezierPatch> b3;
    };
}