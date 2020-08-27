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

    void Scene::Update()
    {
        for (auto torus : m_Torus)
        {
            std::vector<glm::vec3> vertices;
            for (auto point : torus->GetPoints())
            {
                vertices.push_back(point->GetTransform()->Translation); // should be little faster than GetPosition
            }

            auto color = torus->GetIsSelected() ? m_SelectionColor : m_DefaultColor;
            Renderer::RenderTorus(vertices, torus->GetIndices(), torus->GetTransform()->GetMatrix(), color);
        }

        for (auto bezierC0 : m_BezierC0)
        {
            RenderBezier(bezierC0);
        }

        for (auto bSpline : m_BSpline)
        {
            RenderBSpline(bSpline);
        }

        for (auto interpolated : m_InterpolatedCurve)
        {
            RenderInterpolatedCurve(interpolated);
        }

        for (auto bezierPatch : m_BezierPatch)
        {
            RenderBezierPatch(bezierPatch);
        }

        for (auto bSplinePatch : m_BSplinePatch)
        {
            RenderBSplinePatch(bSplinePatch);
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

        for (int i = 0; i < points.size(); i++)
        {
            auto start = points[i]->GetPosition();
            auto end = points[i]->GetPosition();
            Renderer::RenderLine(start, end, color);
        }
    }

    void Scene::RenderControlPolygon(const std::vector<glm::vec3>& points, const glm::vec4& color)
    {
        if (points.empty())
            return;

        for (int i = 0; i < points.size(); i++)
        {
            auto start = points[i];
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
        if (controlPoints.size() < 4)
        {
            RenderControlPoints(controlPoints);
            return;
        }

        auto bezierPoints = bSpline->GetBezierControlPoints();
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

        auto color = bezierPatch->GetIsSelected() ? m_SelectionColor : m_DefaultColor;
        for (int i = 0; i < indices.size(); i += 16)
        {
            Renderer::RenderBezierPatch(
                controlPoints[indices[i]]->GetPosition(),
                controlPoints[indices[i + 1]]->GetPosition(),
                controlPoints[indices[i + 2]]->GetPosition(),
                controlPoints[indices[i + 3]]->GetPosition(),
                controlPoints[indices[i + 4]]->GetPosition(),
                controlPoints[indices[i + 5]]->GetPosition(),
                controlPoints[indices[i + 6]]->GetPosition(),
                controlPoints[indices[i + 7]]->GetPosition(),
                controlPoints[indices[i + 8]]->GetPosition(),
                controlPoints[indices[i + 9]]->GetPosition(),
                controlPoints[indices[i + 10]]->GetPosition(),
                controlPoints[indices[i + 11]]->GetPosition(),
                controlPoints[indices[i + 12]]->GetPosition(),
                controlPoints[indices[i + 13]]->GetPosition(),
                controlPoints[indices[i + 14]]->GetPosition(),
                controlPoints[indices[i + 15]]->GetPosition(),
                bezierPatch->GetUDivisionCount(),
                bezierPatch->GetVDivisionCount(),
                color);
        }

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

        auto color = bSplinePatch->GetIsSelected() ? m_SelectionColor : m_DefaultColor;
        for (int i = 0; i < indices.size(); i += 16)
        {
            Renderer::RenderBSplinePatch(
                controlPoints[indices[i]]->GetPosition(),
                controlPoints[indices[i + 1]]->GetPosition(),
                controlPoints[indices[i + 2]]->GetPosition(),
                controlPoints[indices[i + 3]]->GetPosition(),
                controlPoints[indices[i + 4]]->GetPosition(),
                controlPoints[indices[i + 5]]->GetPosition(),
                controlPoints[indices[i + 6]]->GetPosition(),
                controlPoints[indices[i + 7]]->GetPosition(),
                controlPoints[indices[i + 8]]->GetPosition(),
                controlPoints[indices[i + 9]]->GetPosition(),
                controlPoints[indices[i + 10]]->GetPosition(),
                controlPoints[indices[i + 11]]->GetPosition(),
                controlPoints[indices[i + 12]]->GetPosition(),
                controlPoints[indices[i + 13]]->GetPosition(),
                controlPoints[indices[i + 14]]->GetPosition(),
                controlPoints[indices[i + 15]]->GetPosition(),
                bSplinePatch->GetUDivisionCount(),
                bSplinePatch->GetVDivisionCount(),
                color);
        }

        RenderControlPoints(controlPoints);

        if (bSplinePatch->GetShowPolygon())
        {
            RenderControlGrid(controlPoints, bSplinePatch->GetGridIndices());
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
}