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

        static void Submit(const Ref<OpenGLShader>& shader, const Ref<OpenGLVertexArray>& vertexArray, const glm::mat4& transform = glm::mat4(1.0f), const glm::vec4& color = glm::vec4(1.0f));

    private:
        struct SceneData
        {
            glm::mat4 ViewProjectionMatrix;
        };

        static Scope<SceneData> s_SceneData;
    };
}