#pragma once
#pragma once
#include "Point.h"
#include "Torus.h"
#include "BezierC0.h"
#include "BSpline.h"
#include "InterpolatedCurve.h"
#include "BezierPatch.h"
#include "BSplinePatch.h"
#include "GregoryPatch.h"

namespace CADMageddon
{
    struct PatchRectCreationParameters
    {
        glm::vec3 Position;
        float Width;
        float Height;
        int PatchCountX;
        int PatchCountY;
    };

    struct PatchCylinderCreationParameters
    {
        glm::vec3 Center;
        float Radius;
        float Height;
        int PatchCountX;
        int PatchCountY;
    };

    class SceneSerializer;

    class Scene
    {
    public:
        Scene();
        void Update();
        void DeleteSelected();
        void AssignSelectedFreeToBezier(Ref<BezierC0> bezier);
        void AssignSelectedFreeToBSpline(Ref<BSpline> bSpline);
        void AssignSelectedFreeToInterpolated(Ref<InterpolatedCurve> interpolatedCurve);

        void RemovePointFromBezier(Ref<BezierC0> bezier, Ref<Point> point);
        void RemovePointFromBSpline(Ref<BSpline> bSpline, Ref<Point> point);
        void RemovePointFromInterpolated(Ref<InterpolatedCurve> interpolatedCurve, Ref<Point> point);

        void SetDefaultColor(const glm::vec4& color) { m_DefaultColor = color; }
        void SetSelectionColor(const glm::vec4& color) { m_SelectionColor = color; }

        void MergePoints(Ref<Point> p1, Ref<Point> p2);



        Ref<Point> CreatePoint(glm::vec3 position, std::string name);
        Ref<Torus> CreateTorus(glm::vec3 position, std::string name);

        Ref<BezierC0> CreateBezierC0(std::string name);
        Ref<BSpline> CreateBSpline(std::string name);
        Ref<InterpolatedCurve> CreateInterpolated(std::string name);

        Ref<BezierPatch> CreateBezierPatchRect(std::string name, const PatchRectCreationParameters& parameters);
        Ref<BezierPatch> CreateBezierPatchCylinder(std::string name, const PatchCylinderCreationParameters& parameters);
        Ref<GregoryPatch> CreateGregoryPatch(Ref<BezierPatch> b1, Ref<BezierPatch> b2, Ref<BezierPatch> b3, Ref<Point> commontPoinst[3]);

        Ref<BSplinePatch> CreateBSplinePatchRect(std::string name, const PatchRectCreationParameters& parameters);
        Ref<BSplinePatch> CreateBSplinePatchCylinder(std::string name, const PatchCylinderCreationParameters& parameters);

        std::vector<Ref<Point>> GetFreePoints() const { return m_FreePoints; }
        std::vector<Ref<Point>> GetPoints() const { return m_Points; }
        std::vector<Ref<Torus>> GetTorus() const { return m_Torus; }
        std::vector<Ref<BezierC0>> GetBezierC0() const { return m_BezierC0; }
        std::vector<Ref<BSpline>> GetBSpline() const { return m_BSpline; }
        std::vector<Ref<InterpolatedCurve>> GetInterpolated() const { return m_InterpolatedCurve; }
        std::vector<Ref<BezierPatch>> GetBezierPatch() const { return m_BezierPatch; }
        std::vector<Ref<BSplinePatch>> GetBSplinePatch() const { return m_BSplinePatch; }
        std::vector<Ref<GregoryPatch>> GetGregoryPatch() const { return m_GregoryPatch; }

        void SetOnPointMerged(std::function<void(Ref<Point> p1, Ref<Point> p2, Ref<Point> p3)> onPointMergedCallback)
        {
            m_onPointMerged = onPointMergedCallback;
        }

        void SetOnGregoryPatchDeleted(std::function<void(Ref<GregoryPatch>)> gregoryPatchDeletedCallback)
        {
            m_OnGregoryPatchDeleted = gregoryPatchDeletedCallback;
        }

    private:
        void RenderControlPoints(const std::vector<Ref<Point>>& points);
        void RenderControlPoints(const std::vector<glm::vec3>& points, const glm::vec4& color = glm::vec4(1.0f, 0.0f, 0.0, 1.0f));
        void RenderControlPolygon(const std::vector<Ref<Point>>& points, const glm::vec4& color = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
        void RenderControlPolygon(const std::vector<glm::vec3>& points, const glm::vec4& color = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));

        void RenderControlGrid(const std::vector<Ref<Point>>& points, const std::vector<uint32_t>& gridIndices, const glm::vec4& color = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));

        void RenderBezier(Ref<BezierC0> bezierC0);
        void RenderBSpline(Ref<BSpline> bSpline);
        void RenderInterpolatedCurve(Ref<InterpolatedCurve> interPolatedCurve);
        void RenderBezierPatch(Ref<BezierPatch> bezierPatch);
        void RenderBSplinePatch(Ref<BSplinePatch> bSplinePatch);
        void RenderGregoryPatch(Ref<GregoryPatch> gregoryPatch);
        void RenderGregoryPatch(FillingData fillingData, int uDivisionCount, int vDivisionCount, const glm::vec4& color);

        bool AddNewPointToBezier(Ref<Point> point);
        bool AddNewPointToBSpline(Ref<Point> point);
        bool AddNewPointToInterpolated(Ref<Point> point);

        void DeleteFreePoint(Ref<Point> point);
        void DeleteTorus(Ref<Torus> torus);

        void DeleteBezierC0(Ref<BezierC0> bezierC0);
        void DeleteBSpline(Ref<BSpline> bSpline);
        void DeleteInterpolatedCurve(Ref<InterpolatedCurve> interpolatedCurve);
        void DeleteBezierPatch(Ref<BezierPatch> bezierPatch);
        void DeleteGregoryPatch(Ref<GregoryPatch> gregoryPatch);
        void DeleteBSplinePatch(Ref<BSplinePatch> bSplinePatch);

    private:
        std::unordered_set<Ref<BaseObject>> m_BaseObjects;
        std::vector<Ref<Point>> m_Points; //consider change to unordered set


        std::vector<Ref<Point>> m_FreePoints;
        std::vector<Ref<Torus>> m_Torus;
        std::vector<Ref<BezierC0>> m_BezierC0;
        std::vector<Ref<BSpline>> m_BSpline;
        std::vector < Ref<InterpolatedCurve>> m_InterpolatedCurve;
        std::vector<Ref<BezierPatch>> m_BezierPatch;
        std::vector<Ref<GregoryPatch>> m_GregoryPatch;
        std::vector<Ref<BSplinePatch>> m_BSplinePatch;


        glm::vec4 m_SelectionColor;
        glm::vec4 m_DefaultColor;

        std::function<void(Ref<Point> p1, Ref<Point> p2, Ref<Point> p3)> m_onPointMerged;
        std::function<void(Ref<GregoryPatch>)> m_OnGregoryPatchDeleted;


        friend class SceneSerializer;

    };
}