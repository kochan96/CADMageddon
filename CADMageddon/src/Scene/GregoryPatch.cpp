#include "GregoryPatch.h"

namespace CADMageddon
{
    GregoryPatch::GregoryPatch(std::string name, Border border[3])
        : BaseObject(name)
    {
        m_Border[0] = border[0];
        m_Border[1] = border[1];
        m_Border[2] = border[2];
    }

    Ref<GregoryPatch> CADMageddon::GregoryPatch::Create(
        Ref<BezierPatch> b1,
        Ref<BezierPatch> b2,
        Ref<BezierPatch> b3,
        Ref<Point> commonPoints[3])
    {
        static int gregoryCount = 0;
        Border border[3] =
        {
            GetBorderEnum(b1, commonPoints[2], commonPoints[0]),
            GetBorderEnum(b2, commonPoints[0], commonPoints[1]),
            GetBorderEnum(b3, commonPoints[1], commonPoints[2])
        };

        auto gregory = Ref<GregoryPatch>(new GregoryPatch("Gregory_" + std::to_string(gregoryCount++), border));
        gregory->b1 = b1;
        gregory->b2 = b2;
        gregory->b3 = b3;
        return gregory;
    }

    std::vector<glm::vec3> GregoryPatch::GetSecondHalfBezier(std::vector<glm::vec3> curve)
    {
        for (int i = curve.size() - 1; i > 0; i--)
            for (int j = 0; j < i; j++)
                curve[j] = (curve[j] + curve[j + 1]) / 2.0f;

        return curve;
    }

    std::vector<glm::vec3> GregoryPatch::GetFirstHalfBezier(std::vector<glm::vec3> curve)
    {
        for (int i = 0; i < curve.size() - 1; i++)
            for (int j = curve.size() - 1; j > i; j--)
                curve[j] = (curve[j] + curve[j - 1]) / 2.0f;

        return curve;
    }

    glm::vec3 GregoryPatch::BezierValue(std::vector<glm::vec3> curve, float t)
    {
        for (int i = curve.size() - 1; i > 0; i--)
            for (int j = 0; j < i; j++)
                curve[j] = (curve[j] + curve[j + 1]) / 2.0f;

        return curve[0];
    }

    glm::vec3 GregoryPatch::BezierFirstDerivativeValue(std::vector<glm::vec3> curve, float t)
    {
        for (int i = curve.size() - 1; i > 1; i--)
            for (int j = 0; j < i; j++)
                curve[j] = (curve[j] + curve[j + 1]) / 2.0f;

        return 3.0f * (curve[1] - curve[0]);
    }

    Border GregoryPatch::GetBorderEnum(Ref<BezierPatch> b1, Ref<Point> p0, Ref<Point> p1)
    {
        auto controlPoints = b1->GetControlPoints();
        if (p0 == controlPoints[0])
        {
            if (p1 == controlPoints[3])
                return Border::B03;
            if (p1 == controlPoints[12])
                return Border::B012;
        }
        if (p0 == controlPoints[3])
        {
            if (p1 == controlPoints[0])
                return Border::B30;
            if (p1 == controlPoints[15])
                return Border::B315;
        }
        if (p0 == controlPoints[12])
        {
            if (p1 == controlPoints[15])
                return Border::B1215;
            if (p1 == controlPoints[0])
                return Border::B120;
        }
        if (p0 == controlPoints[15])
        {
            if (p1 == controlPoints[12])
                return Border::B1512;
            if (p1 == controlPoints[3])
                return Border::B153;
        }

        return Border::None;
    }

    std::vector<glm::vec3> GregoryPatch::GetBorderPoints(Ref<BezierPatch> b, Border border)
    {
        std::vector<int> pointIndices;
        switch (border) {
            case Border::B30:
                pointIndices = { 3, 2, 1, 0, 7, 6, 5, 4 };
                break;
            case Border::B012:
                pointIndices = { 0, 4, 8, 12, 1, 5, 9, 13 };
                break;
            case Border::B1215:
                pointIndices = { 12, 13, 14, 15, 8, 9, 10, 11 };
                break;
            case Border::B153:
                pointIndices = { 15, 11, 7, 3, 14, 10, 6, 2 };
                break;
            case Border::B03:
                pointIndices = { 0, 1, 2, 3, 4, 5, 6, 7 };
                break;
            case Border::B120:
                pointIndices = { 12, 8, 4, 0, 13, 9, 5, 1 };
                break;
            case Border::B1512:
                pointIndices = { 15, 14, 13, 12, 11, 10, 9, 8 };
                break;
            case Border::B315:
                pointIndices = { 3, 7, 11, 15, 2, 6, 10, 14 };
                break;
        }

        auto controlPoints = b->GetControlPoints();

        auto result = std::vector<glm::vec3>(pointIndices.size());
        for (int i = 0; i < pointIndices.size(); i++)
            result[i] = controlPoints[pointIndices[i]]->GetPosition();
        return result;
    }

    FillingData GregoryPatch::GetFillingData(
        std::vector<glm::vec3> border,
        std::vector<glm::vec3> borderLeft,
        BoundaryCurve boundaryCurve,
        BoundaryCurve boundaryCurveLeft,
        BoundaryCurve boundaryCurveRight,
        bool showMesh) const
    {
        auto innerBorder = std::vector(border.begin(), border.begin() + 4);
        auto innerBorderLeft = std::vector(borderLeft.begin(), borderLeft.begin() + 4);
        auto outerBorder = std::vector(border.begin() + 4, border.end());
        auto outerBorderLeft = std::vector(borderLeft.begin() + 4, borderLeft.end());

        glm::vec3 a0, a3, b0, b3;

        a0 = BezierFirstDerivativeValue(innerBorder, 0.5f);
        b0 = a0;

        auto centralPoint = boundaryCurve.Fourth;

        a3 = 3.0f * (centralPoint - boundaryCurveRight.Third);
        b3 = 3.0f * (boundaryCurveLeft.Third - centralPoint);

        GFieldVectors gField = GetGField(a0, a3, b0, b3);
        CFieldVectors CField = GetCField(boundaryCurve);

        a0 = -BezierFirstDerivativeValue(innerBorderLeft, 0.5f);
        b0 = a0;

        a3 = 3.0f * (centralPoint - boundaryCurveRight.Third);
        b3 = 3.0f * (boundaryCurve.Third - centralPoint);

        CFieldVectors cFieldLeft = GetCField(boundaryCurveLeft);
        GFieldVectors gFieldLeft = GetGField(a0, a3, b0, b3);
        auto dField = GetDField(gField, CField);
        auto dFieldLeft = GetDField(gFieldLeft, cFieldLeft);

        auto secondHalfBezierInnerP2P0 = GetSecondHalfBezier(innerBorder);
        auto secondHalfBezierOuterP2P0 = GetSecondHalfBezier(outerBorder);
        auto firstHalfBezierInnerP0P1 = GetFirstHalfBezier(innerBorderLeft);
        auto firstHalfBezierOuterP0P1 = GetFirstHalfBezier(outerBorderLeft);

        GregoryPoints gregoryB1B2;
        gregoryB1B2.p0 = secondHalfBezierInnerP2P0[3];
        gregoryB1B2.e0_p = secondHalfBezierInnerP2P0[2];
        gregoryB1B2.e1_m = secondHalfBezierInnerP2P0[1];
        gregoryB1B2.p1 = secondHalfBezierInnerP2P0[0];
        gregoryB1B2.e1_p = boundaryCurve.Second;
        gregoryB1B2.e2_m = boundaryCurve.Third;
        gregoryB1B2.p2 = boundaryCurve.Fourth;
        gregoryB1B2.e2_p = boundaryCurveLeft.Third;
        gregoryB1B2.e3_m = boundaryCurveLeft.Second;
        gregoryB1B2.p3 = boundaryCurveLeft.Bottom;
        gregoryB1B2.e3_p = firstHalfBezierInnerP0P1[2];
        gregoryB1B2.e0_m = firstHalfBezierInnerP0P1[1];

        gregoryB1B2.f0_p = 2.0f * secondHalfBezierInnerP2P0[2] - secondHalfBezierOuterP2P0[2];
        gregoryB1B2.f1_m = 2.0f * secondHalfBezierInnerP2P0[1] - secondHalfBezierOuterP2P0[1];
        gregoryB1B2.f1_p = boundaryCurve.Second + dField.d1;
        gregoryB1B2.f2_m = boundaryCurve.Third + BezierValue({ gField.g0,gField.g1,gField.g2 }, 2.0 / 3.0f);
        gregoryB1B2.f2_p = boundaryCurveLeft.Third + BezierValue({ gFieldLeft.g0,gFieldLeft.g1,gFieldLeft.g2 }, 2.0 / 3.0f);
        gregoryB1B2.f3_m = boundaryCurveLeft.Second + dFieldLeft.d1;
        gregoryB1B2.f3_p = 2.0f * firstHalfBezierInnerP0P1[2] - firstHalfBezierOuterP0P1[2];
        gregoryB1B2.f0_m = 2.0f * firstHalfBezierInnerP0P1[1] - firstHalfBezierOuterP0P1[1];

        gregoryB1B2.f1_p = gregoryB1B2.f1_m;
        gregoryB1B2.f3_m = gregoryB1B2.f3_p;

        FillingData fillingData;
        fillingData.gregoryPoints = gregoryB1B2;

        fillingData.boundaryCurvesPoints = boundaryCurve;
        fillingData.cFieldVectors = CField;
        fillingData.gFieldVectors = gField;
        fillingData.dFieldVectoes = dField;

        fillingData.boundaryCurvesPointsLeft = boundaryCurveLeft;
        fillingData.cFieldVectorsLeft = cFieldLeft;
        fillingData.gFieldVectorsLeft = gFieldLeft;
        fillingData.dFieldVectorsLeft = dFieldLeft;

        fillingData.ShowMesh = showMesh;

        return fillingData;
    }

    BoundaryCurves GregoryPatch::GetBoundaryCurvePoints() const
    {
        BoundaryCurves boundaryPoints;
        auto b1Border = GetBorderPoints(b1, m_Border[0]);
        auto b2Border = GetBorderPoints(b2, m_Border[1]);
        auto b3Border = GetBorderPoints(b3, m_Border[2]);

        auto secondHalfBezierInnerP2P0 = GetSecondHalfBezier(std::vector(b1Border.begin(), b1Border.begin() + 4));
        auto secondHalfBezierOuterP2P0 = GetSecondHalfBezier(std::vector(b1Border.begin() + 4, b1Border.end()));

        auto secondHalfBezierInnerP0P1 = GetSecondHalfBezier(std::vector(b2Border.begin(), b2Border.begin() + 4));
        auto secondHalfBezierOuterP0P1 = GetSecondHalfBezier(std::vector(b2Border.begin() + 4, b2Border.end()));

        auto secondHalfBezierInnerP1P2 = GetSecondHalfBezier(std::vector(b3Border.begin(), b3Border.begin() + 4));
        auto secondHalfBezierOuterP1P2 = GetSecondHalfBezier(std::vector(b3Border.begin() + 4, b3Border.end()));

        BoundaryCurve p1;
        BoundaryCurve p2;
        BoundaryCurve p3;

        p1.Bottom = secondHalfBezierInnerP2P0[0];
        p2.Bottom = secondHalfBezierInnerP0P1[0];
        p3.Bottom = secondHalfBezierInnerP1P2[0];

        p1.Second = 2.0f * secondHalfBezierInnerP2P0[0] - secondHalfBezierOuterP2P0[0];
        p2.Second = 2.0f * secondHalfBezierInnerP0P1[0] - secondHalfBezierOuterP0P1[0];
        p3.Second = 2.0f * secondHalfBezierInnerP1P2[0] - secondHalfBezierOuterP1P2[0];

        auto q0 = (3.0f * p1.Second - p1.Bottom) / 2.0f;
        auto q1 = (3.0f * p2.Second - p2.Bottom) / 2.0f;
        auto q2 = (3.0f * p3.Second - p3.Bottom) / 2.0f;

        auto p_central = (q0 + q1 + q2) / 3.0f;


        p1.Third = (2.0f * q0 + p_central) / 3.0f;
        p2.Third = (2.0f * q1 + p_central) / 3.0f;
        p3.Third = (2.0f * q2 + p_central) / 3.0f;

        p1.Fourth = p_central;
        p2.Fourth = p_central;
        p3.Fourth = p_central;

        boundaryPoints.p1 = p1;
        boundaryPoints.p2 = p2;
        boundaryPoints.p3 = p3;

        boundaryPoints.Center = p_central;

        return boundaryPoints;
    }

    CFieldVectors GregoryPatch::GetCField(BoundaryCurve curve)
    {
        CFieldVectors cField;
        cField.C0 = curve.Bottom;
        cField.C05 = BezierValue({ curve.Bottom,curve.Second,curve.Third,curve.Fourth }, 0.5f);
        cField.C1 = curve.Fourth;
        cField.c0 = curve.Second - curve.Bottom;
        cField.c1 = curve.Third - curve.Second;
        cField.c2 = curve.Fourth - curve.Third;

        return cField;
    }

    GFieldVectors GregoryPatch::GetGField(glm::vec3 a0, glm::vec3 a3, glm::vec3 b0, glm::vec3 b3)
    {
        GFieldVectors gField;
        gField.a0 = a0;
        gField.a3 = a3;
        gField.b0 = b0;
        gField.b3 = b3;

        gField.g0 = (gField.a0 + gField.b0) / 2.0f;
        gField.g2 = (gField.a3 + gField.b3) / 2.0f;
        gField.g1 = (gField.g0 + gField.g2) / 2.0f;

        return gField;
    }

    DFieldVectors GregoryPatch::GetDField(GFieldVectors gField, CFieldVectors cField)
    {
        DFieldVectors b1DField;
        float k0 = (((cField.c0.x * gField.b0.y) - (gField.b0.x * cField.c0.y)) / ((cField.c0.x * gField.g0.y) - (gField.g0.x * cField.c0.y)));
        float h0 = (((gField.b0.x * gField.g0.y) - (gField.b0.y * gField.g0.x)) / ((cField.c0.x * gField.g0.y) - (gField.g0.x * cField.c0.y)));
        float k1 = (((cField.c2.x * gField.b3.y) - (gField.b3.x * cField.c2.y)) / ((cField.c2.x * gField.g2.y) - (gField.g2.x * cField.c2.y)));
        float h1 = (((gField.b3.x * gField.g2.y) - (gField.b3.y * gField.g2.x)) / ((cField.c2.x * gField.g2.y) - (gField.g2.x * cField.c2.y)));

        const float twoThree = 2.0f / 3.0f;
        const float oneThree = 1.0f / 3.0f;

        std::vector<glm::vec3> gFieldVector = { gField.g0,gField.g1,gField.g2 };
        std::vector<glm::vec3> cFieldVector = { cField.c0,cField.C05,cField.c1 };

        b1DField.d0 = k0 * gField.g0 + h0 * cField.c0;
        b1DField.d1 = (k0 * twoThree + k1 * oneThree) * BezierValue(gFieldVector, oneThree)
            + (h0 * twoThree + h1 * oneThree) * BezierValue(cFieldVector, oneThree);
        b1DField.d2 = (k0 * oneThree + k1 * twoThree) * BezierValue(gFieldVector, twoThree)
            + (h0 * oneThree + h1 * twoThree) * BezierValue(cFieldVector, twoThree);
        b1DField.d0 = k1 * gField.g2 + h1 * cField.c2;

        return b1DField;
    }


    FillingData GregoryPatch::GetFillingData(Fill fill) const
    {
        ///      p1
        ///  b2 /  \ b3
        ///    /    \
        //// p0 ---- p2
        ///      b1

        auto boundaryCurves = GetBoundaryCurvePoints();

        auto b1Border = GetBorderPoints(b1, m_Border[0]);
        auto b2Border = GetBorderPoints(b2, m_Border[1]);
        auto b3Border = GetBorderPoints(b3, m_Border[2]);

        std::vector<glm::vec3> border;
        std::vector<glm::vec3> borderLeft;

        BoundaryCurve boundaryCurve, boundaryCurveLeft, boundaryCurveRight;
        bool showMesh = false;

        switch (fill)
        {
            case CADMageddon::Fill::B12:
            {
                border = b1Border;
                borderLeft = b2Border;
                boundaryCurve = boundaryCurves.p1;
                boundaryCurveLeft = boundaryCurves.p2;
                boundaryCurveRight = boundaryCurves.p3;
                showMesh = m_ShowFirstMesh;
            }
            break;
            case CADMageddon::Fill::B23:
            {
                border = b2Border;
                borderLeft = b3Border;
                boundaryCurve = boundaryCurves.p2;
                boundaryCurveLeft = boundaryCurves.p3;
                boundaryCurveRight = boundaryCurves.p1;
                showMesh = m_ShowSecondMesh;
                break;
            }
            case CADMageddon::Fill::B31:
            {
                border = b3Border;
                borderLeft = b1Border;
                boundaryCurve = boundaryCurves.p3;
                boundaryCurveLeft = boundaryCurves.p1;
                boundaryCurveRight = boundaryCurves.p2;
                showMesh = m_ShowThirdMesh;
                break;
            }
            default:
                break;
        }

        GregoryCorner points[4];

        auto curveIn = GetSecondHalfBezier(
            std::vector<glm::vec3>(b1Border.begin(), b1Border.begin() + 4));
        std::reverse(curveIn.begin(), curveIn.end());
        auto curveOut = GetSecondHalfBezier(
            std::vector<glm::vec3>(b1Border.begin() + 4, b1Border.begin() + 8));
        std::reverse(curveOut.begin(), curveOut.end());

        for (int i = 0; i < 4; i++)
            curveOut[i] = 2.0f * curveIn[i] - curveOut[i];
        points[0].p = curveIn[0];
        points[0].e[1] = curveIn[1];
        points[1].e[0] = curveIn[2];
        points[1].p = curveIn[3];
        points[0].f[1] = curveOut[1];
        points[1].f[0] = points[1].f[1] = curveOut[2];
        points[1].e[1] = curveOut[3];

        curveIn = GetSecondHalfBezier(
            std::vector<glm::vec3>(b2Border.begin(), b2Border.begin() + 4));
        curveOut = GetSecondHalfBezier(
            std::vector<glm::vec3>(b2Border.begin() + 4, b2Border.begin() + 8));
        for (int i = 0; i < 4; i++)
            curveOut[i] = 2.0f * curveIn[i] - curveOut[i];
        points[0].e[0] = curveIn[1];
        points[3].e[1] = curveIn[2];
        points[3].p = curveIn[3];
        points[0].f[0] = curveOut[1];
        points[3].f[0] = points[3].f[1] = curveOut[2];
        points[3].e[0] = curveOut[3];

        curveIn = GetSecondHalfBezier(
            std::vector<glm::vec3>(b3Border.begin(), b3Border.begin() + 4));
        curveOut = GetSecondHalfBezier(
            std::vector<glm::vec3>(b3Border.begin() + 4, b3Border.begin() + 8));
        curveOut[3] = 2.0f * curveIn[3] - curveOut[3];
        curveOut[3] = (3.0f * curveOut[3] - curveIn[3]) / 2.f;
        points[2].e[0] = (3.f * points[1].e[1] - points[1].p) / 2.f;
        points[2].e[1] = (3.f * points[3].e[0] - points[3].p) / 2.f;
        points[2].p = (curveOut[3] + points[2].e[0] + points[2].e[1]) / 3.f;
        points[2].e[0] = (2.f * points[2].e[0] + points[2].p) / 3.f;
        points[2].e[1] = (2.f * points[2].e[1] + points[2].p) / 3.f;
        curveOut[3] = (2.f * curveOut[3] + points[2].p) / 3.f;
        glm::vec3 g[3];
        g[0] = (points[2].e[0] - curveOut[3]) / 2.f;
        g[2] = points[3].e[1] - points[3].p;
        g[1] = (g[0] + g[2]) / 2.f;
        points[2].f[1] = 1.f / 9.f * g[2] + 4.f / 9.f * (g[0] + g[1]) + points[2].e[1];

        g[0] = (points[2].e[1] - curveOut[3]) / 2.f;
        g[2] = points[1].e[0] - points[1].p;
        g[1] = (g[0] + g[2]) / 2.f;
        points[2].f[0] = 1.f / 9.f * g[2] + 4.f / 9.f * (g[0] + g[1]) + points[2].e[0];

        return GetFillingData(border, borderLeft, boundaryCurve, boundaryCurveLeft, boundaryCurveRight, showMesh);
    }
}
