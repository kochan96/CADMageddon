#pragma once
#include "VertexArray.h"
#include "Shader.h"
#include "Camera.h"

namespace CADMageddon
{
    class Renderer
    {
    public:
        //static constexpr glm::vec4 DEFAULT_COLOR = glm::vec4(0.05f, 0.08f, 0.71f, 1.0f);
        static constexpr glm::vec4 DEFAULT_COLOR = glm::vec4(0.0f, 0.8f, 0.0f, 1.0f);
        static constexpr glm::vec4 SELECTED_COLOR = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);

        static void Init();
        static void ShutDown();
        static void OnWindowResize(uint32_t width, uint32_t height);

        static void BeginScene(FPSCamera& camera);
        static void EndScene();
        static void RenderGrid(const Ref<OpenGLVertexArray>& vertexArray, const glm::mat4& transform, const glm::vec4& color = DEFAULT_COLOR);
        static void RenderTorus(
            const std::vector<glm::vec3>& vertices,
            const std::vector<uint32_t>& indices,
            const glm::mat4& transform,
            const glm::vec4& color = DEFAULT_COLOR);

        static void RenderPoint(const glm::vec3& position, const glm::vec4& color = DEFAULT_COLOR);
        static void RenderLine(const glm::vec3& start, const glm::vec3& end, const glm::vec4& color = DEFAULT_COLOR);

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

    private:
        static void InitTorusRenderData();
        static void InitPointRenderData();
        static void InitLineRenderData();
        static void InitBezierRenderData();

        static void FlushAndResetPoints();
        static void FlushAndResetLines();

        static void FlushPoints();
        static void FlushLines();

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