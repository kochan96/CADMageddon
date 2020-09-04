#pragma once
#include "VertexArray.h"
#include "Shader.h"
#include "Camera.h"

namespace CADMageddon
{
    class Renderer
    {
    public:

        static int PointSize;
        static bool ShowPoints;

        static constexpr glm::vec4 DEFAULT_COLOR = glm::vec4(0.0f, 0.8f, 0.0f, 1.0f);
        static constexpr glm::vec4 SELECTED_COLOR = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);

        static void Init();
        static void ShutDown();
        static void OnWindowResize(uint32_t width, uint32_t height);

        static void BeginScene(const glm::mat4& viewProjectionMatrix);
        static void EndScene();
        static void RenderGrid(const Ref<OpenGLVertexArray>& vertexArray, const glm::mat4& transform, const glm::vec4& color = DEFAULT_COLOR);
        static void RenderTorus(
            const std::vector<glm::vec3>& vertices,
            const std::vector<glm::vec2>& textureCoordinates,
            const std::vector<uint32_t>& indices,
            const glm::mat4& transform,
            const glm::vec4& color = DEFAULT_COLOR);

        static void RenderTrimmedTorus(
            const std::vector<glm::vec3>& vertices,
            const std::vector<glm::vec2>& textureCoordinates,
            const bool reverseTrimming,
            const unsigned int textureId,
            const std::vector<uint32_t>& indices,
            const glm::mat4& transform,
            const glm::vec4& color = DEFAULT_COLOR);

        static void RenderPoint(const glm::vec3& position, const glm::vec4& color = DEFAULT_COLOR);
        static void RenderLine(const glm::vec3& start, const glm::vec3& end, const glm::vec4& color = DEFAULT_COLOR);

        static void RenderScreenQuad(const glm::vec2& bottomLeft, const glm::vec2& topRight, const glm::vec4& color = DEFAULT_COLOR);
        static void RenderScreenQuadBorder(const glm::vec2& bottomLeft, const glm::vec2& topRight, const glm::vec4& color = DEFAULT_COLOR);

        static void RenderBezierC0(
            const glm::vec3& p0,
            const glm::vec3& p1,
            const glm::vec3& p2,
            const glm::vec4& color = DEFAULT_COLOR,
            float tolerance = 1.0005f
        );

        static void RenderBezierC0(
            const glm::vec3& p0,
            const glm::vec3& p1,
            const glm::vec3& p2,
            const glm::vec3& p3,
            const glm::vec4& color = DEFAULT_COLOR,
            float tolerance = 1.0005f
        );

        static void ShaderRenderBezierC0(
            const std::vector<glm::vec3>& controlPoints,
            const glm::vec4& color = DEFAULT_COLOR);

        static void ShaderRenderBezierC0(
            const glm::vec3& p0,
            const glm::vec3& p1,
            const glm::vec3& p2,
            const glm::vec4& color = DEFAULT_COLOR);

        static void ShaderRenderBezierC0(
            const glm::vec3& p0,
            const glm::vec3& p1,
            const glm::vec3& p2,
            const glm::vec3& p3,
            const glm::vec4& color = DEFAULT_COLOR);


        static void RenderBezierPatch(
            const std::vector<glm::vec3>& vertices,
            const std::vector<uint32_t>& indices,
            const std::vector<glm::vec2>& textureCoordinates,
            float uSubdivisionCount,
            float vSubdivionCount,
            int patchCountx,
            int patchCounty,
            bool isTrimmed,
            unsigned int textureId,
            bool reverseTrimming,
            const glm::vec4& color = DEFAULT_COLOR);

        static void RenderBSplinePatch(
            const std::vector<glm::vec3>& vertices,
            const std::vector<uint32_t>& indices,
            const std::vector<glm::vec2>& textureCoordinates,
            float uSubdivisionCount,
            float vSubdivionCount,
            int patchCountx,
            int patchCounty,
            bool isTrimmed,
            unsigned int textureId,
            bool reverseTrimming,
            const glm::vec4& color = DEFAULT_COLOR);

        static void RenderGregoryPatch(
            const glm::vec3& p0,
            const glm::vec3& e0_m,
            const glm::vec3& e0_p,
            const glm::vec3& f0_m,
            const glm::vec3& f0_p,
            const glm::vec3& p1,
            const glm::vec3& e1_m,
            const glm::vec3& e1_p,
            const glm::vec3& f1_m,
            const glm::vec3& f1_p,
            const glm::vec3& p2,
            const glm::vec3& e2_m,
            const glm::vec3& e2_p,
            const glm::vec3& f2_m,
            const glm::vec3& f2_p,
            const glm::vec3& p3,
            const glm::vec3& e3_m,
            const glm::vec3& e3_p,
            const glm::vec3& f3_m,
            const glm::vec3& f3_p,
            int uSubdivisionCount,
            int vSubdivisionCount,
            const glm::vec4& color = DEFAULT_COLOR
        );

        static void RenderGregoryPatch(
            glm::vec3* data,
            int uSubDivisionCount,
            int vSubdivisionCount,
            const glm::vec4& color = DEFAULT_COLOR);

        static void RenderBSpline(
            const glm::vec3& p0,
            const glm::vec3& p1,
            const glm::vec3& p2,
            const glm::vec3& p3,
            const glm::vec4& color = DEFAULT_COLOR,
            bool snapToEnd = false);

    private:
        static void InitTorusRenderData();
        static void InitPointRenderData();
        static void InitLineRenderData();
        static void InitBezierCurveRenderData();
        static void InitBezierPatchRenderData();
        static void InitBSplinePatchRenderData();
        static void InitSelectionBoxRenderData();
        static void InitGregoryPatchRenderData();

        static void FlushAndResetPoints();
        static void FlushAndResetLines();

        static void FlushPoints();
        static void FlushLines();

        static float Spline(float t, float ti, float interval = 1.0f);
        static float Spline1(float t, float ti, float interval = 1.0f);
        static float Spline2(float t, float ti, float interval = 1.0f);
        static float Spline3(float t, float ti, float interval = 1.0f);
        static glm::vec4 SplineBasis(float t);

        struct SceneData
        {
            glm::mat4 ViewProjectionMatrix;
        };

        static Scope<SceneData> s_SceneData;
        static Scope<ShaderLibrary> s_ShaderLibrary;

        static bool IsBezierFlatEnough(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, float tolerance);
        static bool IsBezierFlatEnough(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, float tolerance);
    };
}