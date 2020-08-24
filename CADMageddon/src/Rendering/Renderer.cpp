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
    static RenderLineData s_RenderLineData;
    static RenderBezierData s_RenderBezierData;

    void Renderer::Init()
    {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(OpenGLMessageCallback, nullptr);

        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        /*glEnable(GL_BLEND);
        glBlendFunc(GL_DST_ALPHA, GL_ALWA);*/

        s_ShaderLibrary->Load("FlatColorShader", "assets/shaders/FlatColorShader.glsl");
        s_ShaderLibrary->Load("ColorShader", "assets/shaders/ColorShader.glsl");
        s_ShaderLibrary->Load("CubicBezierCurveShader", "assets/shaders/CubicBezierCurveShader.glsl");
        s_ShaderLibrary->Load("QuadraticBezierCurveShader", "assets/shaders/QuadraticBezierCurveShader.glsl");

        InitTorusRenderData();
        InitPointRenderData();
        InitLineRenderData();
        InitBezierRenderData();
    }

    void Renderer::InitTorusRenderData()
    {
        s_RenderTorusData.TorusVertexArray = CreateRef<OpenGLVertexArray>();
        s_RenderTorusData.TorusVertexBuffer = CreateRef<OpenGLVertexBuffer>(s_RenderTorusData.MaxVertices * sizeof(Vertex));
        s_RenderTorusData.TorusIndexBuffer = CreateRef<OpenGLIndexBuffer>(s_RenderTorusData.MaxIndices);

        s_RenderTorusData.TorusVertexBuffer->SetLayout({
            { ShaderDataType::Float3, "a_Position" }
            });

        s_RenderTorusData.TorusVertexArray->AddVertexBuffer(s_RenderTorusData.TorusVertexBuffer);
        s_RenderTorusData.TorusVertexArray->SetIndexBuffer(s_RenderTorusData.TorusIndexBuffer);
        s_RenderTorusData.FlatColorShader = s_ShaderLibrary->Get("FlatColorShader");
    }

    void Renderer::InitPointRenderData()
    {
        s_RenderPointData.PointsVertexArray = CreateRef<OpenGLVertexArray>();

        s_RenderPointData.PointsVertexBuffer = CreateRef<OpenGLVertexBuffer>(s_RenderPointData.MaxPoints * sizeof(VertexC));
        s_RenderPointData.PointsVertexBuffer->SetLayout({
            { ShaderDataType::Float3, "a_Position" },
            { ShaderDataType::Float4, "a_Color" }
            });

        s_RenderPointData.PointsVertexArray->AddVertexBuffer(s_RenderPointData.PointsVertexBuffer);

        s_RenderPointData.Shader = s_ShaderLibrary->Get("ColorShader");

        s_RenderPointData.PointVertexBufferBase = new VertexC[s_RenderPointData.MaxPoints];

        glPointSize(10.0f);
    }

    void Renderer::InitLineRenderData()
    {
        s_RenderLineData.LinesVertexArray = CreateRef<OpenGLVertexArray>();

        s_RenderLineData.LinesVertexBuffer = CreateRef<OpenGLVertexBuffer>(s_RenderLineData.MaxLines * sizeof(VertexC));
        s_RenderLineData.LinesVertexBuffer->SetLayout({
            { ShaderDataType::Float3, "a_Position" },
            { ShaderDataType::Float4, "a_Color" }
            });

        s_RenderLineData.LinesVertexArray->AddVertexBuffer(s_RenderLineData.LinesVertexBuffer);

        s_RenderLineData.Shader = s_ShaderLibrary->Get("ColorShader");

        s_RenderLineData.LinesVertexBufferBase = new VertexC[s_RenderLineData.MaxLines];

        //glLineWidth(10.0f);
    }

    void Renderer::InitBezierRenderData()
    {
        s_RenderBezierData.BezierVertexArray = CreateRef<OpenGLVertexArray>();

        s_RenderBezierData.BezierVertexBuffer = CreateRef<OpenGLVertexBuffer>(s_RenderBezierData.MaxPoints * sizeof(Vertex));
        s_RenderBezierData.BezierVertexBuffer->SetLayout({
            { ShaderDataType::Float3, "a_Position" },
            });

        s_RenderBezierData.BezierVertexArray->AddVertexBuffer(s_RenderBezierData.BezierVertexBuffer);

        s_RenderBezierData.QuadraticBezierShader = s_ShaderLibrary->Get("QuadraticBezierCurveShader");
        s_RenderBezierData.CubicBezierShader = s_ShaderLibrary->Get("CubicBezierCurveShader");
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

        s_RenderLineData.LinesVertexBufferPtr = s_RenderLineData.LinesVertexBufferBase;
        s_RenderLineData.Count = 0;
    }

    void Renderer::EndScene()
    {
        uint32_t pointDataSize = (uint32_t)((uint8_t*)s_RenderPointData.PointVertexBufferPtr - (uint8_t*)s_RenderPointData.PointVertexBufferBase);
        s_RenderPointData.PointsVertexBuffer->SetData(s_RenderPointData.PointVertexBufferBase, pointDataSize);

        FlushPoints();

        uint32_t lineDataSize = (uint32_t)((uint8_t*)s_RenderLineData.LinesVertexBufferPtr - (uint8_t*)s_RenderLineData.LinesVertexBufferBase);
        s_RenderLineData.LinesVertexBuffer->SetData(s_RenderLineData.LinesVertexBufferBase, lineDataSize);

        FlushLines();
    }

    void Renderer::RenderTorus(
        const std::vector<glm::vec3>& vertices,
        const::std::vector<uint32_t>& indices,
        const glm::mat4& transform,
        const glm::vec4& color)
    {

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        s_RenderTorusData.FlatColorShader->Bind();
        s_RenderTorusData.FlatColorShader->SetMat4("u_ViewProjectionMatrix", s_SceneData->ViewProjectionMatrix);
        s_RenderTorusData.FlatColorShader->SetMat4("u_ModelMatrix", transform);
        s_RenderTorusData.FlatColorShader->SetFloat4("u_Color", color);

        s_RenderTorusData.TorusVertexArray->Bind();
        s_RenderTorusData.TorusVertexBuffer->SetData(&vertices[0].x, vertices.size() * sizeof(Vertex));
        s_RenderTorusData.TorusIndexBuffer->SetIndices(indices.data(), indices.size());

        glDrawElements(GL_LINES, s_RenderTorusData.TorusVertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    void Renderer::RenderGrid(const Ref<OpenGLVertexArray>& vertexArray, const glm::mat4& transform, const glm::vec4& color)
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

    void Renderer::RenderPoint(const glm::vec3& position, const glm::vec4& color)
    {
        if (s_RenderPointData.Count >= s_RenderPointData.MaxPoints)
            FlushAndResetPoints();

        s_RenderPointData.PointVertexBufferPtr->Position = position;
        s_RenderPointData.PointVertexBufferPtr->Color = color;
        s_RenderPointData.PointVertexBufferPtr++;

        s_RenderPointData.Count++;
    }

    void Renderer::RenderLine(const glm::vec3& start, const glm::vec3& end, const glm::vec4& color)
    {
        if (s_RenderLineData.Count >= s_RenderLineData.MaxLines)
            FlushAndResetLines();

        s_RenderLineData.LinesVertexBufferPtr->Position = start;
        s_RenderLineData.LinesVertexBufferPtr->Color = color;
        s_RenderLineData.LinesVertexBufferPtr++;

        s_RenderLineData.Count++;

        s_RenderLineData.LinesVertexBufferPtr->Position = end;
        s_RenderLineData.LinesVertexBufferPtr->Color = color;
        s_RenderLineData.LinesVertexBufferPtr++;

        s_RenderLineData.Count++;
    }

    void Renderer::RenderBezierC0(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, const glm::vec4& color, float tolerance)
    {
        if (IsBezierFlatEnough(p0, p1, p2, tolerance))
        {
            Renderer::RenderLine(p0, p2, color);
        }
        else
        {
            const float t = 0.5f;

            glm::vec3 p01 = mix(p0, p1, t);
            glm::vec3 p12 = mix(p1, p2, t);
            glm::vec3 p0112 = mix(p01, p12, t);
            Renderer::RenderBezierC0(p0, p01, p0112, color, tolerance);
            Renderer::RenderBezierC0(p0112, p12, p2, color, tolerance);
        }
    }

    void Renderer::RenderBezierC0(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, const glm::vec4& color, float tolerance)
    {
        if (IsBezierFlatEnough(p0, p1, p2, p3, tolerance))
        {
            Renderer::RenderLine(p0, p3, color);
        }
        else
        {
            const float t = 0.5f;

            glm::vec3 p01 = mix(p0, p1, t);
            glm::vec3 p12 = mix(p1, p2, t);
            glm::vec3 p23 = mix(p2, p3, t);

            glm::vec3 p0112 = mix(p01, p12, t);
            glm::vec3 p1223 = mix(p12, p23, t);

            glm::vec3 p01121223 = mix(p0112, p1223, t);

            Renderer::RenderBezierC0(p0, p01, p0112, p01121223, color, tolerance);
            Renderer::RenderBezierC0(p01121223, p1223, p23, p3, color, tolerance);
        }
    }


    void Renderer::ShaderRenderBezierC0(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, const glm::vec4& color)
    {
        float vertices[] = { p0.x,p0.y,p0.z,p1.x,p1.y,p1.z,p2.x,p2.y,p2.z };

        s_RenderBezierData.BezierVertexArray->Bind();
        s_RenderBezierData.BezierVertexBuffer->SetData(vertices, sizeof(float) * 9);
        s_RenderBezierData.QuadraticBezierShader->Bind();
        s_RenderBezierData.QuadraticBezierShader->SetFloat4("u_Color", color);
        s_RenderBezierData.QuadraticBezierShader->SetMat4("u_ViewProjectionMatrix", s_SceneData->ViewProjectionMatrix);

        glDrawArrays(GL_TRIANGLES, 0, 3);

    }

    void Renderer::ShaderRenderBezierC0(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, const glm::vec4& color)
    {
        float vertices[] = { p0.x,p0.y,p0.z,p1.x,p1.y,p1.z,p2.x,p2.y,p2.z,p3.x,p3.y,p3.z };

        s_RenderBezierData.BezierVertexArray->Bind();
        s_RenderBezierData.BezierVertexBuffer->SetData(vertices, sizeof(float) * 12);
        s_RenderBezierData.CubicBezierShader->Bind();
        s_RenderBezierData.CubicBezierShader->SetFloat4("u_Color", color);
        s_RenderBezierData.CubicBezierShader->SetMat4("u_ViewProjectionMatrix", s_SceneData->ViewProjectionMatrix);

        glDrawArrays(GL_LINES_ADJACENCY, 0, 4);
    }

    void Renderer::FlushAndResetPoints()
    {
        uint32_t pointDataSize = (uint32_t)((uint8_t*)s_RenderPointData.PointVertexBufferPtr - (uint8_t*)s_RenderPointData.PointVertexBufferBase);
        s_RenderPointData.PointsVertexBuffer->SetData(s_RenderPointData.PointVertexBufferBase, pointDataSize);
        FlushPoints();

        s_RenderPointData.Count = 0;
        s_RenderPointData.PointVertexBufferPtr = s_RenderPointData.PointVertexBufferBase;

    }

    void Renderer::FlushAndResetLines()
    {
        uint32_t lineDataSize = (uint32_t)((uint8_t*)s_RenderLineData.LinesVertexBufferPtr - (uint8_t*)s_RenderLineData.LinesVertexBufferBase);
        s_RenderLineData.LinesVertexBuffer->SetData(s_RenderLineData.LinesVertexBufferBase, lineDataSize);
        FlushLines();

        s_RenderLineData.Count = 0;
        s_RenderLineData.LinesVertexBufferPtr = s_RenderLineData.LinesVertexBufferBase;
    }

    void Renderer::FlushPoints()
    {
        if (s_RenderPointData.Count == 0)
            return;

        s_RenderPointData.PointsVertexArray->Bind();
        s_RenderPointData.Shader->Bind();
        s_RenderPointData.Shader->SetMat4("u_ViewProjectionMatrix", s_SceneData->ViewProjectionMatrix);

        glDrawArrays(GL_POINTS, 0, s_RenderPointData.Count);
    }

    void Renderer::FlushLines()
    {
        if (s_RenderLineData.Count == 0)
            return;

        s_RenderLineData.LinesVertexArray->Bind();
        s_RenderLineData.Shader->Bind();
        s_RenderLineData.Shader->SetMat4("u_ViewProjectionMatrix", s_SceneData->ViewProjectionMatrix);

        glDrawArrays(GL_LINES, 0, s_RenderLineData.Count);
    }
    bool Renderer::IsBezierFlatEnough(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, float tolerance)
    {
        return glm::distance(p0, p1) + glm::distance(p1, p2) < tolerance * glm::distance(p0, p2);
    }

    bool Renderer::IsBezierFlatEnough(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, float tolerance)
    {
        return glm::distance(p0, p1) + glm::distance(p1, p2) + glm::distance(p2, p3) < tolerance * glm::distance(p0, p3);
    }
}
