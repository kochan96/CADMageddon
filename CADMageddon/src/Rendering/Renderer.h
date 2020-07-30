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
        static void Init();
        static void ShutDown();
        static void OnWindowResize(uint32_t width, uint32_t height);

        static void BeginScene(FPSCamera& camera);
        static void EndScene();

        static void RenderTorus(const Mesh& mesh, const glm::mat4& transform, const glm::vec4& color);
        static void RenderGrid(const Ref<OpenGLVertexArray> vertexArray, const glm::mat4& transform, const glm::vec4& color);
        static void RenderPoint(const PointComponent& pointComponent, const glm::mat4& transform, const glm::vec4& color);

    private:

        static void FlushAndReset();
        static void Flush();

        struct SceneData
        {
            glm::mat4 ViewProjectionMatrix;
        };

        static Scope<SceneData> s_SceneData;
        static Scope<ShaderLibrary> s_ShaderLibrary;
    };
}