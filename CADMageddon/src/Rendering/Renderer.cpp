#include <cadpch.h>
#include "Renderer.h"

#include "RenderData.h"
#include <glad\glad.h>

namespace CADMageddon
{
    void OpenGLMessageCallback(
        unsigned source,
        unsigned type,
        unsigned id,
        unsigned severity,
        int length,
        const char* message,
        const void* userParam)
    {
        switch (severity)
        {
        case GL_DEBUG_SEVERITY_HIGH:         LOG_CRITITCAL(message); return;
        case GL_DEBUG_SEVERITY_MEDIUM:       LOG_ERROR(message); return;
        case GL_DEBUG_SEVERITY_LOW:          LOG_WARNING(message); return;
        case GL_DEBUG_SEVERITY_NOTIFICATION: LOG_TRACE(message); return;
        }
    }

    Scope<Renderer::SceneData> Renderer::s_SceneData = CreateScope<Renderer::SceneData>();
    Scope<ShaderLibrary> Renderer::s_ShaderLibrary = CreateScope<ShaderLibrary>();

    static RenderTorusData s_RenderTorusData;
    static RenderPointData s_RenderPointData;

    void Renderer::Init()
    {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(OpenGLMessageCallback, nullptr);

        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glEnable(GL_DEPTH_TEST);

        s_ShaderLibrary->Load("FlatColorShader", "assets/shaders/FlatColorShader.glsl");
        s_ShaderLibrary->Load("PointsShader", "assets/shaders/PointsShader.glsl");

        s_RenderTorusData.TorusVertexArray = CreateRef<OpenGLVertexArray>();
        s_RenderTorusData.TorusVertexBuffer = CreateRef<OpenGLVertexBuffer>(s_RenderTorusData.MaxVertices * sizeof(Vertex));
        s_RenderTorusData.TorusIndexBuffer = CreateRef<OpenGLIndexBuffer>(s_RenderTorusData.MaxIndices);

        s_RenderTorusData.TorusVertexBuffer->SetLayout({
            { ShaderDataType::Float3, "a_Position" }
            });

        s_RenderTorusData.TorusVertexArray->AddVertexBuffer(s_RenderTorusData.TorusVertexBuffer);
        s_RenderTorusData.TorusVertexArray->SetIndexBuffer(s_RenderTorusData.TorusIndexBuffer);
        s_RenderTorusData.FlatColorShader = s_ShaderLibrary->Get("FlatColorShader");

        s_RenderPointData.PointsVertexArray = CreateRef<OpenGLVertexArray>();

        s_RenderPointData.PointsVertexBuffer = CreateRef<OpenGLVertexBuffer>(s_RenderPointData.MaxPoints * sizeof(VertexC));
        s_RenderPointData.PointsVertexBuffer->SetLayout({
            { ShaderDataType::Float3, "a_Position" },
            { ShaderDataType::Float4, "a_Color" }
            });

        s_RenderPointData.PointsVertexArray->AddVertexBuffer(s_RenderPointData.PointsVertexBuffer);

        s_RenderPointData.Shader = s_ShaderLibrary->Get("PointsShader");

        s_RenderPointData.PointVertexBufferBase = new VertexC[s_RenderPointData.MaxPoints];

        glPointSize(10.0f);
    }

    void Renderer::ShutDown()
    {
    }

    void Renderer::OnWindowResize(uint32_t width, uint32_t height)
    {
        glViewport(0, 0, width, height);
    }

    void Renderer::BeginScene(FPSCamera& camera)
    {
        s_SceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();

        s_RenderPointData.PointVertexBufferPtr = s_RenderPointData.PointVertexBufferBase;

        s_RenderPointData.Count = 0;
    }

    void Renderer::EndScene()
    {
        uint32_t dataSize = (uint32_t)((uint8_t*)s_RenderPointData.PointVertexBufferPtr - (uint8_t*)s_RenderPointData.PointVertexBufferBase);
        s_RenderPointData.PointsVertexBuffer->SetData(s_RenderPointData.PointVertexBufferBase, dataSize);

        Flush();
    }

    void Renderer::RenderTorus(const Mesh& mesh, const glm::mat4& transform, const glm::vec4& color)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        s_RenderTorusData.FlatColorShader->Bind();
        s_RenderTorusData.FlatColorShader->SetMat4("u_ViewProjectionMatrix", s_SceneData->ViewProjectionMatrix);
        s_RenderTorusData.FlatColorShader->SetMat4("u_ModelMatrix", transform);
        s_RenderTorusData.FlatColorShader->SetFloat4("u_Color", color);

        s_RenderTorusData.TorusVertexArray->Bind();
        s_RenderTorusData.TorusVertexBuffer->SetData(&mesh.Vertices[0].x, mesh.Vertices.size() * sizeof(Vertex));
        s_RenderTorusData.TorusIndexBuffer->SetIndices(mesh.Indices.data(), mesh.Indices.size());

        glDrawElements(GL_LINES, s_RenderTorusData.TorusVertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    void Renderer::RenderGrid(const Ref<OpenGLVertexArray> vertexArray, const glm::mat4& transform, const glm::vec4& color)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        auto shader = s_ShaderLibrary->Get("FlatColorShader");
        shader->Bind();
        shader->SetMat4("u_ViewProjectionMatrix", s_SceneData->ViewProjectionMatrix);
        shader->SetMat4("u_ModelMatrix", transform);
        shader->SetFloat4("u_Color", color);

        vertexArray->Bind();
        glDrawElements(GL_LINES, vertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    void Renderer::RenderPoint(const PointComponent& pointComponent, const glm::mat4& transform, const glm::vec4& color)
    {
        if (s_RenderPointData.Count >= s_RenderPointData.MaxPoints)
            FlushAndReset();

        s_RenderPointData.PointVertexBufferPtr->Position = transform * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        s_RenderPointData.PointVertexBufferPtr->Color = color;
        s_RenderPointData.PointVertexBufferPtr++;

        s_RenderPointData.Count++;
    }

    void Renderer::FlushAndReset()
    {
        EndScene();

        s_RenderPointData.Count = 0;
        s_RenderPointData.PointVertexBufferPtr = s_RenderPointData.PointVertexBufferBase;

    }

    void Renderer::Flush()
    {
        if (s_RenderPointData.Count == 0)
            return;

        s_RenderPointData.PointsVertexArray->Bind();
        s_RenderPointData.Shader->Bind();
        s_RenderPointData.Shader->SetMat4("u_ViewProjectionMatrix", s_SceneData->ViewProjectionMatrix);

        glDrawArrays(GL_POINTS, 0, s_RenderPointData.Count);
    }
}
