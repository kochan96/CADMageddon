#pragma once
#include "VertexArray.h"
#include "Shader.h"
#include "Camera.h"

namespace CADMageddon
{
    class Renderer
    {
    public:
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
            const std::vector<uint32_t>& indices,
            const glm::mat4& transform,
            const glm::vec4& color = DEFAULT_COLOR);

        static void RenderPoint(const glm::vec3& position, const glm::vec4& color = DEFAULT_COLOR);
        static void RenderLine(const glm::vec3& start, const glm::vec3& end, const glm::vec4& color = DEFAULT_COLOR);

        static void RenderScreenQuad(const glm::vec2& bottomLeft, const glm::vec2& topRight, const glm::vec4& color = DEFAULT_COLOR);
        static void RenderScreenQuadBorder(const glm::vec2& bottomLeft, const glm::vec2& topRight,const glm::vec4& color = DEFAULT_COLOR);

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
            const glm::vec3& p0,
            const glm::vec3& p1,
            const glm::vec3& p2,
            const glm::vec3& p3,
            const glm::vec3& p4,
            const glm::vec3& p5,
            const glm::vec3& p6,
            const glm::vec3& p7,
            const glm::vec3& p8,
            const glm::vec3& p9,
            const glm::vec3& p10,
            const glm::vec3& p11,
            const glm::vec3& p12,
            const glm::vec3& p13,
            const glm::vec3& p14,
            const glm::vec3& p15,
            float uSubdivisionCount,
            float vSubdivionCount,
            const glm::vec4& color = DEFAULT_COLOR
        );

        static void RenderBSplinePatch(
            const glm::vec3& p0,
            const glm::vec3& p1,
            const glm::vec3& p2,
            const glm::vec3& p3,
            const glm::vec3& p4,
            const glm::vec3& p5,
            const glm::vec3& p6,
            const glm::vec3& p7,
            const glm::vec3& p8,
            const glm::vec3& p9,
            const glm::vec3& p10,
            const glm::vec3& p11,
            const glm::vec3& p12,
            const glm::vec3& p13,
            const glm::vec3& p14,
            const glm::vec3& p15,
            float uSubdivisionCount,
            float vSubdivionCount,
            const glm::vec4& color = DEFAULT_COLOR
        );

        static void RenderBSpline(
            const glm::vec3& p0,
            const glm::vec3& p1,
            const glm::vec3& p2,
            const glm::vec3& p3,
            const glm::vec4& color = DEFAULT_COLOR);

    private:
        static void InitTorusRenderData();
        static void InitPointRenderData();
        static void InitLineRenderData();
        static void InitBezierCurveRenderData();
        static void InitBezierPatchRenderData();
        static void InitBSplinePatchRenderData();
        static void InitSelectionBoxRenderData();

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