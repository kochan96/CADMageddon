#pragma once
#include "VertexArray.h"
#include "Shader.h"
#include "Camera.h"

#include "Scene\Components.h"

namespace CADMageddon
{
    class Renderer
    {
    public:
        static constexpr glm::vec4 DEFAULT_COLOR = glm::vec4(0.05f, 0.08f, 0.71f, 1.0f);
        static constexpr glm::vec4 SELECTED_COLOR = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);

        static void Init();
        static void ShutDown();
        static void OnWindowResize(uint32_t width, uint32_t height);

        static void BeginScene(FPSCamera& camera);
        static void EndScene();
        static void RenderGrid(const Ref<OpenGLVertexArray>& vertexArray, const glm::mat4& transform, const glm::vec4& color = DEFAULT_COLOR);
        static void RenderTorus(const Mesh& mesh, const glm::mat4& transform, const glm::vec4& color = DEFAULT_COLOR);
        static void RenderPoint(const glm::vec3& position, const glm::vec4& color = DEFAULT_COLOR);
        static void RenderLine(const glm::vec3& start, const glm::vec3& end, const glm::vec4& color = DEFAULT_COLOR);

    private:
        static void InitTorusRenderData();
        static void InitPointRenderData();
        static void InitLineRenderData();


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
    };
}