#include "Scene.h"
#include "Rendering\Renderer.h"

namespace CADMageddon
{
    Scene::Scene()
    {
        m_SelectionColor = Renderer::SELECTED_COLOR;
        m_DefaultColor = Renderer::DEFAULT_COLOR;
    }

    static int pointCount = 0;

    void Scene::MergePoints(Ref<Point> p1, Ref<Point> p2)
    {
        if (std::find_if(m_FreePoints.begin(), m_FreePoints.end(), [p1, p2](Ref<Point> p) {return p == p1 || p == p2; }) != m_FreePoints.end())
            return;

        static int mergedCount = 0;
        auto newName = "MergedPoint_" + std::to_string(mergedCount++);
        auto newPosition = (p1->GetPosition() + p2->GetPosition()) / 2.0f;

        auto newPoint = CreateRef<Point>(newPosition, newName);
        int referenced = 0;

        for (auto baseObject : m_BaseObjects)
        {
            auto& points = baseObject->GetControlPoints();
            auto it = std::find(points.begin(), points.end(), p1);
            auto it2 = std::find(points.begin(), points.end(), p2);
            int  referencedIncrement = 0;

            while (it != points.end() || it2 != points.end())
            {
                referencedIncrement = 1;

                if (it != points.end())
                {
                    *it = newPoint;
                    it = std::find(it + 1, points.end(), p1);
                }
                if (it2 != points.end())
                {
                    *it2 = newPoint;
                    it2 = std::find(it2 + 1, points.end(), p2);
                }
            }
            referenced += referencedIncrement;
        }

        auto it = std::find(m_Points.begin(), m_Points.end(), p1);
        *it = newPoint;
        it = std::find(m_Points.begin(), m_Points.end(), p2);
        m_Points.erase(it);
        newPoint->SetReferencedCount(referenced);

        if (m_onPointMerged)
            m_onPointMerged(p1, p2, newPoint);
    }

    Ref<Point> Scene::CreatePoint(glm::vec3 position, std::string name)
    {
        auto point = CreateRef<Point>(position, name + std::to_string(pointCount++));
        m_Points.push_back(point);
        auto added = AddNewPointToBezier(point);
        if (!added)
            added = AddNewPointToBSpline(point);
        if (!added)
            added = AddNewPointToInterpolated(point);

        if (!added)
            m_FreePoints.push_back(point);

        return point;
    }

    Ref<Torus> Scene::CreateTorus(glm::vec3 position, std::string name)
    {
        static int torusCount = 0;
        auto torus = CreateRef<Torus>(position, name + std::to_string(torusCount++));
        m_Torus.push_back(torus);
        return torus;
    }

    Ref<BezierC0> Scene::CreateBezierC0(std::string name)
    {
        static int bezierC0Count = 0;
        auto bezierC0 = CreateRef<BezierC0>(name + std::to_string(bezierC0Count++));
        m_BezierC0.push_back(bezierC0);
        m_BaseObjects.insert(bezierC0);
        return bezierC0;
    }

    Ref<BSpline> Scene::CreateBSpline(std::string name)
    {
        static int bSplineCount = 0;
        auto bSpline = CreateRef<BSpline>(name + std::to_string(bSplineCount++));
        m_BSpline.push_back(bSpline);
        m_BaseObjects.insert(bSpline);
        return bSpline;
    }

    Ref<InterpolatedCurve> Scene::CreateInterpolated(std::string name)
    {
        static int interpolatedCount = 0;
        auto interpolated = CreateRef<InterpolatedCurve>(name + std::to_string(interpolatedCount++));
        m_InterpolatedCurve.push_back(interpolated);
        m_BaseObjects.insert(interpolated);
        return interpolated;
    }

    Ref<InterpolatedCurve> Scene::CreateInterpolated(Ref<InterpolatedCurve> curve)
    {
        for (auto point : curve->GetControlPoints())
        {
            m_Points.push_back(point);
        }

        m_InterpolatedCurve.push_back(curve);
        m_BaseObjects.insert(curve);
        return curve;
    }

    static int bezierPatchCount = 0;

    Ref<BezierPatch> Scene::CreateBezierPatchRect(std::string name, const PatchRectCreationParameters& parameters)
    {
        auto bezier = BezierPatch::CreateRectPatch(name + std::to_string(bezierPatchCount++), parameters.Position, parameters.PatchCountX, parameters.PatchCountY, parameters.Width, parameters.Height);
        for (auto controlPoints : bezier->GetControlPoints())
        {
            m_Points.push_back(controlPoints);
        }

        m_BezierPatch.push_back(bezier);
        m_BaseObjects.insert(bezier);

        return bezier;
    }

    Ref<BezierPatch> Scene::CreateBezierPatchCylinder(std::string name, const PatchCylinderCreationParameters& parameters)
    {
        auto bezier = BezierPatch::CreateCyliderPatch(name + std::to_string(bezierPatchCount++), parameters.Center, parameters.PatchCountX, parameters.PatchCountY, parameters.Radius, parameters.Height);
        for (auto controlPoints : bezier->GetControlPoints())
        {
            m_Points.push_back(controlPoints);
        }

        m_BezierPatch.push_back(bezier);
        m_BaseObjects.insert(bezier);

        return bezier;
    }

    Ref<GregoryPatch> Scene::CreateGregoryPatch(Ref<BezierPatch> b1, Ref<BezierPatch> b2, Ref<BezierPatch> b3, Ref<Point> commonPoints[3])
    {
        auto gregory = GregoryPatch::Create(b1, b2, b3, commonPoints);
        m_GregoryPatch.push_back(gregory);

        return gregory;
    }

    static int bSplinePatchCount = 0;

    Ref<BSplinePatch> Scene::CreateBSplinePatchRect(std::string name, const PatchRectCreationParameters& parameters)
    {
        auto bSpline = BSplinePatch::CreateRectPatch(name + std::to_string(bSplinePatchCount++), parameters.Position, parameters.PatchCountX, parameters.PatchCountY, parameters.Width, parameters.Height);
        for (auto controlPoints : bSpline->GetControlPoints())
        {
            m_Points.push_back(controlPoints);
        }

        m_BSplinePatch.push_back(bSpline);
        m_BaseObjects.insert(bSpline);


        return bSpline;
    }

    Ref<BSplinePatch> Scene::CreateBSplinePatchCylinder(std::string name, const PatchCylinderCreationParameters& parameters)
    {
        auto bSpline = BSplinePatch::CreateCyliderPatch(name + std::to_string(bSplinePatchCount++), parameters.Center, parameters.PatchCountX, parameters.PatchCountY, parameters.Radius, parameters.Height);
        for (auto controlPoints : bSpline->GetControlPoints())
        {
            m_Points.push_back(controlPoints);
        }

        m_BSplinePatch.push_back(bSpline);
        m_BaseObjects.insert(bSpline);

        return bSpline;
    }

    Ref<IntersectionCurve> Scene::CreateIntersectionCurve(std::string name, Ref<SurfaceUV> s1, Ref<SurfaceUV> s2, std::vector<IntersectionPoint> points, IntersectionType intersectionType)
    {
        static int intersectionCount = 0;
        auto intersectionCurve = IntersectionCurve::Create("Intersection_" + std::to_string(intersectionCount++), points, s1, s2, intersectionType);
        m_IntersectionCurve.push_back(intersectionCurve);


        if (intersectionType == IntersectionType::ClosedClosed || intersectionType == IntersectionType::ClosedOpen)
        {
            s1->SetIntersectionCurve(intersectionCurve);
            s1->SetReverseTrimming(true);
        }

        if (intersectionType == IntersectionType::ClosedClosed || intersectionType == IntersectionType::OpenClosed)
        {
            s2->SetIntersectionCurve(intersectionCurve);
        }

        return intersectionCurve;
    }

    void Scene::Update()
    {
        for (auto torus : m_Torus)
        {
            RenderTorus(torus);
        }

        for (auto bezierC0 : m_BezierC0)
        {
            if (bezierC0->GetIsVisible())
                RenderBezier(bezierC0);
        }

        for (auto bSpline : m_BSpline)
        {
            if (bSpline->GetIsVisible())
                RenderBSpline(bSpline);
        }

        for (auto interpolated : m_InterpolatedCurve)
        {
            if (interpolated->GetIsVisible())
                RenderInterpolatedCurve(interpolated);
        }

        for (auto bezierPatch : m_BezierPatch)
        {
            if (bezierPatch->GetIsVisible())
                RenderBezierPatch(bezierPatch);
        }

        for (auto bSplinePatch : m_BSplinePatch)
        {
            if (bSplinePatch->GetIsVisible())
                RenderBSplinePatch(bSplinePatch);
        }

        for (auto gregoryPatch : m_GregoryPatch)
        {
            if (gregoryPatch->GetIsVisible())
                RenderGregoryPatch(gregoryPatch);
        }

        for (auto intersectionCurve : m_IntersectionCurve)
        {
            if (intersectionCurve->GetIsVisible())
                RenderIntersectionCurve(intersectionCurve);
        }

        RenderControlPoints(m_FreePoints);
    }

    void Scene::DeleteSelected()
    {
        std::vector<Ref<Torus>> torusToDelete;
        std::vector<Ref<BezierC0>> bezierC0ToDelete;
        std::vector<Ref<BSpline>> bSplineToDelete;
        std::vector<Ref<InterpolatedCurve>> interpolatedToDelete;

        auto points = m_FreePoints;
        auto toruses = m_Torus;
        auto beziers = m_BezierC0;
        auto splines = m_BSpline;
        auto interpolatedCurves = m_InterpolatedCurve;
        auto bezierPatches = m_BezierPatch;
        auto bSplinePatches = m_BSplinePatch;
        auto gregoryPatches = m_GregoryPatch;
        auto intersectionCurves = m_IntersectionCurve;

        for (auto point : points)
        {
            if (point->GetIsSelected())
                DeleteFreePoint(point);
        }

        for (auto torus : toruses)
        {
            if (torus->GetIsSelected())
                DeleteTorus(torus);
        }

        for (auto bezierC0 : beziers)
        {
            if (bezierC0->GetIsSelected())
                DeleteBezierC0(bezierC0);
        }

        for (auto bSpline : splines)
        {
            if (bSpline->GetIsSelected())
                DeleteBSpline(bSpline);
        }

        for (auto interpolated : interpolatedCurves)
        {
            if (interpolated->GetIsSelected())
                DeleteInterpolatedCurve(interpolated);
        }

        for (auto bezierPatch : bezierPatches)
        {
            if (bezierPatch->GetIsSelected())
                DeleteBezierPatch(bezierPatch);
        }

        for (auto bSplinePatch : bSplinePatches)
        {
            if (bSplinePatch->GetIsSelected())
                DeleteBSplinePatch(bSplinePatch);
        }

        for (auto gregoryPatch : gregoryPatches)
        {
            if (gregoryPatch->GetIsSelected())
                DeleteGregoryPatch(gregoryPatch);
        }

        for (auto intersected : intersectionCurves)
        {
            if (intersected->GetIsSelected())
                DeleteIntersectionCurve(intersected);
        }
    }

    void Scene::AssignSelectedFreeToBezier(Ref<BezierC0> bezier)
    {
        auto points = m_FreePoints;
        for (auto point : points)
        {
            if (point->GetIsSelected())
            {
                bezier->AddControlPoint(point);
                auto it = std::find(m_FreePoints.begin(), m_FreePoints.end(), point);
                m_FreePoints.erase(it);
            }
        }
    }

    void Scene::AssignSelectedFreeToBSpline(Ref<BSpline> bSpline)
    {
        auto points = m_FreePoints;
        for (auto point : points)
        {
            if (point->GetIsSelected())
            {
                bSpline->AddControlPoint(point);
                auto it = std::find(m_FreePoints.begin(), m_FreePoints.end(), point);
                m_FreePoints.erase(it);
            }
        }
    }

    void Scene::AssignSelectedFreeToInterpolated(Ref<InterpolatedCurve> interpolatedCurve)
    {
        auto points = m_FreePoints;
        for (auto point : points)
        {
            if (point->GetIsSelected())
            {
                interpolatedCurve->AddControlPoint(point);
                auto it = std::find(m_FreePoints.begin(), m_FreePoints.end(), point);
                m_FreePoints.erase(it);
            }
        }
    }

    void Scene::RemovePointFromBezier(Ref<BezierC0> bezier, Ref<Point> point)
    {
        bezier->RemoveControlPoint(point);
        if (point->GetReferencedCount() == 0)
            m_FreePoints.push_back(point);
    }

    void Scene::RemovePointFromBSpline(Ref<BSpline> bSpline, Ref<Point> point)
    {
        bSpline->RemoveControlPoint(point);
        if (point->GetReferencedCount() == 0)
            m_FreePoints.push_back(point);
    }

    void Scene::RemovePointFromInterpolated(Ref<InterpolatedCurve> interpolatedCurve, Ref<Point> point)
    {
        interpolatedCurve->RemoveControlPoint(point);
        if (point->GetReferencedCount() == 0)
            m_FreePoints.push_back(point);
    }

    bool Scene::AddNewPointToBezier(Ref<Point> point)
    {
        bool added = false;
        for (auto bezier : m_BezierC0)
        {
            if (bezier->GetIsSelected())
            {
                added = true;
                bezier->AddControlPoint(point);
                return true;
            }
        }


        return added;
    }

    bool Scene::AddNewPointToBSpline(Ref<Point> point)
    {
        bool added = false;
        for (auto bSpline : m_BSpline)
        {
            if (bSpline->GetIsSelected())
            {
                added = true;
                bSpline->AddControlPoint(point);
                return true;
            }
        }


        return added;
    }

    bool Scene::AddNewPointToInterpolated(Ref<Point> point)
    {
        bool added = false;
        for (auto interpolated : m_InterpolatedCurve)
        {
            if (interpolated->GetIsSelected())
            {
                added = true;
                interpolated->AddControlPoint(point);
                return true;
            }
        }

        return added;
    }

    void Scene::RenderControlPoints(const std::vector<Ref<Point>>& points)
    {
        for (auto point : points)
        {
            auto color = point->GetIsSelected() ? m_SelectionColor : m_DefaultColor;
            if (point->GetIsVisible())
                Renderer::RenderPoint(point->GetPosition(), color);
        }
    }

    void Scene::RenderControlPoints(const std::vector<glm::vec3>& points, const glm::vec4& color)
    {
        for (auto point : points)
        {
            Renderer::RenderPoint(point, color);
        }
    }

    void Scene::RenderControlPolygon(const std::vector<Ref<Point>>& points, const glm::vec4& color)
    {
        if (points.empty())
            return;

        for (int i = 1; i < points.size(); i++)
        {
            auto start = points[i - 1]->GetPosition();
            auto end = points[i]->GetPosition();
            Renderer::RenderLine(start, end, color);
        }
    }

    void Scene::RenderControlPolygon(const std::vector<glm::vec3>& points, const glm::vec4& color)
    {
        if (points.empty())
            return;

        for (int i = 1; i < points.size(); i++)
        {
            auto start = points[i - 1];
            auto end = points[i];
            Renderer::RenderLine(start, end, color);
        }
    }

    void Scene::RenderControlGrid(const std::vector<Ref<Point>>& points, const std::vector<uint32_t>& gridIndices, const glm::vec4& color)
    {
        for (int i = 0; i < gridIndices.size() - 1; i += 2)
        {
            auto color = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
            Renderer::RenderLine(points[gridIndices[i]]->GetPosition(), points[gridIndices[i + 1]]->GetPosition(), color);
        }
    }

    void Scene::RenderTorus(Ref<Torus> torus)
    {

        std::vector<glm::vec3> vertices;
        for (auto point : torus->GetPoints())
        {
            vertices.push_back(point->GetTransform()->Translation);
        }

        auto color = torus->GetIsSelected() ? m_SelectionColor : m_DefaultColor;

        if (!torus->GetIsTrimmed())
        {
            Renderer::RenderTorus(vertices, torus->GetTextureCoordinates(), torus->GetIndices(), torus->GetTransform()->GetMatrix(), color);
        }
        else
        {
            bool isFirst = torus->GetIntersectionCurve()->GetFirstSurface() == torus;
            unsigned int textureId = 0;
            if (isFirst)
            {
                textureId = torus->GetIntersectionCurve()->GetFirstTextureId();
            }
            else
            {
                textureId = torus->GetIntersectionCurve()->GetSecondTextureId();
            }

            Renderer::RenderTrimmedTorus(
                vertices,
                torus->GetTextureCoordinates(),
                torus->GetReverseTrimming(),
                textureId,
                torus->GetIndices(),
                torus->GetTransform()->GetMatrix(),
                color);
        }
    }

    void Scene::RenderBezier(Ref<BezierC0> bezierC0)
    {
        auto controlPoints = bezierC0->GetControlPoints();
        if (controlPoints.empty())
            return;

        std::vector<glm::vec3> controlPointsPositions;
        std::transform(controlPoints.begin(), controlPoints.end(), std::back_inserter(controlPointsPositions), [](Ref<Point> p) {return p->GetPosition(); });

        RenderControlPoints(controlPoints);

        if (bezierC0->GetShowPolygon())
        {
            RenderControlPolygon(controlPoints);
        }

        auto bezierColor = bezierC0->GetIsSelected() ? m_SelectionColor : m_DefaultColor;
        Renderer::ShaderRenderBezierC0(controlPointsPositions, bezierColor);
    }

    void Scene::RenderBSpline(Ref<BSpline> bSpline)
    {
        auto controlPoints = bSpline->GetControlPoints();
        auto bezierPoints = bSpline->GetBezierControlPoints();
        if (bezierPoints.empty())
        {
            RenderControlPoints(controlPoints);
            return;
        }

        auto bSplineColor = bSpline->GetIsSelected() ? m_SelectionColor : m_DefaultColor;

        RenderControlPoints(controlPoints);
        Renderer::ShaderRenderBezierC0(bezierPoints, bSplineColor);

        if (bSpline->GetShowBSplinePolygon())
        {
            RenderControlPolygon(controlPoints);
        }

        if (bSpline->GetIsBezierBasis())
        {
            RenderControlPoints(bezierPoints);

            if (bSpline->GetShowBezierPolygon())
            {
                RenderControlPolygon(bezierPoints);
            }
        }
    }

    void Scene::RenderInterpolatedCurve(Ref<InterpolatedCurve> interPolatedCurve)
    {
        auto controlPoints = interPolatedCurve->GetControlPoints();
        auto bezierPoints = interPolatedCurve->GetBezierControlPoints();
        auto interpolatedColor = interPolatedCurve->GetIsSelected() ? m_SelectionColor : m_DefaultColor;

        RenderControlPoints(controlPoints);
        Renderer::ShaderRenderBezierC0(bezierPoints, interpolatedColor);

        if (interPolatedCurve->GetShowPolygon())
        {
            RenderControlPolygon(controlPoints);
        }
    }

    void Scene::RenderBezierPatch(Ref<BezierPatch> bezierPatch)
    {
        auto indices = bezierPatch->GetRenderingIndices();
        auto controlPoints = bezierPatch->GetControlPoints();
        auto vertices = bezierPatch->GetRenderingVertices();
        auto textureCooridnates = bezierPatch->GetTextureCoordinates();
        auto color = bezierPatch->GetIsSelected() ? m_SelectionColor : m_DefaultColor;

        Renderer::RenderBezierPatch(
            vertices,
            indices,
            textureCooridnates,
            bezierPatch->GetUDivisionCount(),
            bezierPatch->GetVDivisionCount(),
            bezierPatch->GetPatchCountX(),
            bezierPatch->GetPatchCountY(),
            bezierPatch->GetIsTrimmed(),
            bezierPatch->GetTextureId(),
            bezierPatch->GetReverseTrimming(),
            color);

        RenderControlPoints(controlPoints);

        if (bezierPatch->GetShowPolygon())
        {
            RenderControlGrid(controlPoints, bezierPatch->GetGridIndices());
        }
    }

    void Scene::RenderBSplinePatch(Ref<BSplinePatch> bSplinePatch)
    {
        auto indices = bSplinePatch->GetRenderingIndices();
        auto controlPoints = bSplinePatch->GetControlPoints();
        auto vertices = bSplinePatch->GetRenderingVertices();
        auto textureCooridnates = bSplinePatch->GetTextureCoordinates();
        auto color = bSplinePatch->GetIsSelected() ? m_SelectionColor : m_DefaultColor;

        Renderer::RenderBSplinePatch(
            vertices,
            indices,
            textureCooridnates,
            bSplinePatch->GetUDivisionCount(),
            bSplinePatch->GetVDivisionCount(),
            bSplinePatch->GetPatchCountX(),
            bSplinePatch->GetPatchCountY(),
            bSplinePatch->GetIsTrimmed(),
            bSplinePatch->GetTextureId(),
            bSplinePatch->GetReverseTrimming(),
            color);

        RenderControlPoints(controlPoints);

        if (bSplinePatch->GetShowPolygon())
        {
            RenderControlGrid(controlPoints, bSplinePatch->GetGridIndices());
        }
    }

    void Scene::RenderGregoryPatch(Ref<GregoryPatch> gregoryPatch)
    {
        auto color = gregoryPatch->GetIsSelected() ? m_SelectionColor : m_DefaultColor;
        int uDivisionCount = gregoryPatch->GetUDivisionCount();
        int vDivisionCount = gregoryPatch->GetVDivisionCount();
        glm::vec4 blue = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);

        if (gregoryPatch->GetShowFirst() || gregoryPatch->GetShowFirstMesh())
        {
            auto fillingData = gregoryPatch->GetFillingData(Fill::B12);
            if (gregoryPatch->GetShowFirst())
                RenderGregoryPatch(fillingData, uDivisionCount, vDivisionCount, gregoryPatch->GetShowFirstMesh(), color);
            if (gregoryPatch->GetShowFirstMesh())
                RenderGregoryPatchMesh(fillingData, blue);

        }
        if (gregoryPatch->GetShowSecond() || gregoryPatch->GetShowSecondMesh())
        {
            auto fillingData = gregoryPatch->GetFillingData(Fill::B23);
            if (gregoryPatch->GetShowSecond())
                RenderGregoryPatch(fillingData, uDivisionCount, vDivisionCount, gregoryPatch->GetShowFirstMesh(), color);
            if (gregoryPatch->GetShowSecondMesh())
                RenderGregoryPatchMesh(fillingData, blue);
        }
        if (gregoryPatch->GetShowThird() || gregoryPatch->GetShowThirdMesh())
        {
            auto fillingData = gregoryPatch->GetFillingData(Fill::B31);
            if (gregoryPatch->GetShowThird())
                RenderGregoryPatch(fillingData, uDivisionCount, vDivisionCount, gregoryPatch->GetShowFirstMesh(), color);
            if (gregoryPatch->GetShowThirdMesh())
                RenderGregoryPatchMesh(fillingData, blue);
        }
    }

    void Scene::RenderGregoryPatch(FillingData fillingData, int uDivisionCount, int vDivisionCount, bool showMesh, const glm::vec4& color)
    {
        Renderer::RenderGregoryPatch(
            fillingData.gregoryPoints.p0,
            fillingData.gregoryPoints.e0_m,
            fillingData.gregoryPoints.e0_p,
            fillingData.gregoryPoints.f0_m,
            fillingData.gregoryPoints.f0_p,
            fillingData.gregoryPoints.p1,
            fillingData.gregoryPoints.e1_m,
            fillingData.gregoryPoints.e1_p,
            fillingData.gregoryPoints.f1_m,
            fillingData.gregoryPoints.f1_p,
            fillingData.gregoryPoints.p2,
            fillingData.gregoryPoints.e2_m,
            fillingData.gregoryPoints.e2_p,
            fillingData.gregoryPoints.f2_m,
            fillingData.gregoryPoints.f2_p,
            fillingData.gregoryPoints.p3,
            fillingData.gregoryPoints.e3_m,
            fillingData.gregoryPoints.e3_p,
            fillingData.gregoryPoints.f3_m,
            fillingData.gregoryPoints.f3_p,
            uDivisionCount,
            vDivisionCount,
            color);
    }

    void Scene::RenderGregoryPatchMesh(FillingData fillingData, const glm::vec4& color)
    {
        Renderer::RenderLine(fillingData.gregoryPoints.p0, fillingData.gregoryPoints.e0_m, color);
        Renderer::RenderLine(fillingData.gregoryPoints.p0, fillingData.gregoryPoints.e0_p, color);
        Renderer::RenderLine(fillingData.gregoryPoints.e0_m, fillingData.gregoryPoints.f0_m, color);
        Renderer::RenderLine(fillingData.gregoryPoints.e0_p, fillingData.gregoryPoints.f0_p, color);

        Renderer::RenderLine(fillingData.gregoryPoints.e0_p, fillingData.gregoryPoints.e1_m, color);

        Renderer::RenderLine(fillingData.gregoryPoints.p1, fillingData.gregoryPoints.e1_m, color);
        Renderer::RenderLine(fillingData.gregoryPoints.p1, fillingData.gregoryPoints.e1_p, color);
        Renderer::RenderLine(fillingData.gregoryPoints.e1_m, fillingData.gregoryPoints.f1_m, color);
        Renderer::RenderLine(fillingData.gregoryPoints.e1_p, fillingData.gregoryPoints.f1_p, color);

        Renderer::RenderLine(fillingData.gregoryPoints.e1_p, fillingData.gregoryPoints.e2_m, color);

        Renderer::RenderLine(fillingData.gregoryPoints.p2, fillingData.gregoryPoints.e2_m, color);
        Renderer::RenderLine(fillingData.gregoryPoints.p2, fillingData.gregoryPoints.e2_p, color);
        Renderer::RenderLine(fillingData.gregoryPoints.e2_m, fillingData.gregoryPoints.f2_m, color);
        Renderer::RenderLine(fillingData.gregoryPoints.e2_p, fillingData.gregoryPoints.f2_p, color);

        Renderer::RenderLine(fillingData.gregoryPoints.e2_p, fillingData.gregoryPoints.e3_m, color);

        Renderer::RenderLine(fillingData.gregoryPoints.p3, fillingData.gregoryPoints.e3_m, color);
        Renderer::RenderLine(fillingData.gregoryPoints.p3, fillingData.gregoryPoints.e3_p, color);
        Renderer::RenderLine(fillingData.gregoryPoints.e3_m, fillingData.gregoryPoints.f3_m, color);
        Renderer::RenderLine(fillingData.gregoryPoints.e3_p, fillingData.gregoryPoints.f3_p, color);

        Renderer::RenderLine(fillingData.gregoryPoints.e3_p, fillingData.gregoryPoints.e0_m, color);
    }

    void Scene::RenderIntersectionCurve(Ref<IntersectionCurve> curve)
    {
        auto points = curve->GetIntersectionPoints();
        auto color = curve->GetIsSelected() ? m_SelectionColor : m_DefaultColor;
        for (int i = 1; i < points.size(); i++)
        {
            auto start = points[i - 1];
            auto end = points[i];
            Renderer::RenderLine(start.Location, end.Location, color);
        }
    }

    void Scene::DeleteFreePoint(Ref<Point> point)
    {
        {
            auto it = std::find(m_Points.begin(), m_Points.end(), point);
            if (it != m_Points.end())
            {
                m_Points.erase(it);
            }
        }
        {
            auto it = std::find(m_FreePoints.begin(), m_FreePoints.end(), point);
            if (it != m_FreePoints.end())
            {
                m_FreePoints.erase(it);
            }
        }
    }

    void Scene::DeleteTorus(Ref<Torus> torus)
    {
        auto it = std::find(m_Torus.begin(), m_Torus.end(), torus);
        if (it != m_Torus.end())
        {
            m_Torus.erase(it);
        }
    }

    void Scene::DeleteBezierC0(Ref<BezierC0> bezierC0)
    {
        auto it = std::find(m_BezierC0.begin(), m_BezierC0.end(), bezierC0);
        if (it != m_BezierC0.end())
        {
            for (auto point : bezierC0->GetControlPoints())
            {
                int pointReferenceCount = point->GetReferencedCount();
                point->SetReferencedCount(pointReferenceCount - 1);

                if (pointReferenceCount == 1)
                    m_FreePoints.push_back(point);
            }

            m_BezierC0.erase(it);
        }

        m_BaseObjects.erase(bezierC0);
    }

    void Scene::DeleteBSpline(Ref<BSpline> bSpline)
    {
        auto it = std::find(m_BSpline.begin(), m_BSpline.end(), bSpline);
        if (it != m_BSpline.end())
        {
            for (auto point : bSpline->GetControlPoints())
            {
                int pointReferenceCount = point->GetReferencedCount();
                point->SetReferencedCount(pointReferenceCount - 1);

                if (pointReferenceCount == 1)
                    m_FreePoints.push_back(point);
            }

            m_BSpline.erase(it);
        }

        m_BaseObjects.erase(bSpline);

    }

    void Scene::DeleteInterpolatedCurve(Ref<InterpolatedCurve> interpolatedCurve)
    {
        auto it = std::find(m_InterpolatedCurve.begin(), m_InterpolatedCurve.end(), interpolatedCurve);
        if (it != m_InterpolatedCurve.end())
        {
            for (auto point : interpolatedCurve->GetControlPoints())
            {
                int pointReferenceCount = point->GetReferencedCount();
                point->SetReferencedCount(pointReferenceCount - 1);

                if (pointReferenceCount == 1)
                    m_FreePoints.push_back(point);
            }

            m_InterpolatedCurve.erase(it);
        }

        m_BaseObjects.erase(interpolatedCurve);

    }

    void Scene::DeleteBezierPatch(Ref<BezierPatch> bezierPatch)
    {
        auto it = std::find(m_BezierPatch.begin(), m_BezierPatch.end(), bezierPatch);
        if (it != m_BezierPatch.end())
        {
            for (auto point : bezierPatch->GetControlPoints())
            {
                int pointReferenceCount = point->GetReferencedCount();
                point->SetReferencedCount(pointReferenceCount - 1);

                if (pointReferenceCount == 1)
                    m_FreePoints.push_back(point);
            }

            m_BezierPatch.erase(it);
        }

        m_BaseObjects.erase(bezierPatch);
        auto gregoryPatches = m_GregoryPatch;

        for (auto gregory : gregoryPatches)
        {
            if (gregory->GetB1() == bezierPatch || gregory->GetB2() == bezierPatch || gregory->GetB3() == bezierPatch)
            {
                auto it = std::find(m_GregoryPatch.begin(), m_GregoryPatch.end(), gregory);
                if (it != m_GregoryPatch.end())
                    m_GregoryPatch.erase(it);
                if (m_OnGregoryPatchDeleted)
                    m_OnGregoryPatchDeleted(gregory);
            }
        }
    }

    void Scene::DeleteGregoryPatch(Ref<GregoryPatch> gregoryPatch)
    {
        auto it = std::find(m_GregoryPatch.begin(), m_GregoryPatch.end(), gregoryPatch);
        if (it != m_GregoryPatch.end())
            m_GregoryPatch.erase(it);
    }

    void Scene::DeleteBSplinePatch(Ref<BSplinePatch> bSplinePatch)
    {
        auto it = std::find(m_BSplinePatch.begin(), m_BSplinePatch.end(), bSplinePatch);
        if (it != m_BSplinePatch.end())
        {
            for (auto point : bSplinePatch->GetControlPoints())
            {
                int pointReferenceCount = point->GetReferencedCount();
                point->SetReferencedCount(pointReferenceCount - 1);

                if (pointReferenceCount == 1)
                    m_FreePoints.push_back(point);
            }

            m_BSplinePatch.erase(it);
        }

        m_BaseObjects.erase(bSplinePatch);
    }

    void Scene::DeleteIntersectionCurve(Ref<IntersectionCurve> curve)
    {
        auto it = std::find(m_IntersectionCurve.begin(), m_IntersectionCurve.end(), curve);
        if (it != m_IntersectionCurve.end())
        {
            m_IntersectionCurve.erase(it);
            curve->GetFirstSurface()->SetIntersectionCurve(nullptr);
            curve->GetSecondSurface()->SetIntersectionCurve(nullptr);
        }
    }
}