#include <cadpch.h>
#include "Renderer.h"

#include "RenderData.h"
#include <glad\glad.h>

namespace CADMageddon
{

    float Renderer::PointSize = 10.0f;
    bool Renderer::ShowPoints = true;

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
    static RenderBezierCurveData s_RenderBezierCurveData;
    static RenderBezierPatchData s_RenderBezierPatchData;
    static RenderBSplinePatchData s_RenderBSplinePatchData;
    static RenderSelectionBoxData s_RenderSelectionBoxData;

    void Renderer::Init()
    {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(OpenGLMessageCallback, nullptr);

        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        s_ShaderLibrary->Load("FlatColorShader", "assets/shaders/FlatColorShader.glsl");
        s_ShaderLibrary->Load("ColorShader", "assets/shaders/ColorShader.glsl");
        s_ShaderLibrary->Load("CubicBezierCurveShader", "assets/shaders/CubicBezierCurveShader.glsl");
        s_ShaderLibrary->Load("QuadraticBezierCurveShader", "assets/shaders/QuadraticBezierCurveShader.glsl");
        s_ShaderLibrary->Load("BezierPatchShader", "assets/shaders/BezierPatchShader.glsl");
        s_ShaderLibrary->Load("BSplinePatchShader", "assets/shaders/BSplinePatchShader.glsl");
        s_ShaderLibrary->Load("SelectionBoxShader", "assets/shaders/SelectionBoxShader.glsl");

        InitTorusRenderData();
        InitPointRenderData();
        InitLineRenderData();
        InitBezierCurveRenderData();
        InitBezierPatchRenderData();
        InitBSplinePatchRenderData();
        InitSelectionBoxRenderData();
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

        glPointSize(PointSize);
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

    void Renderer::InitBezierCurveRenderData()
    {
        s_RenderBezierCurveData.BezierVertexArray = CreateRef<OpenGLVertexArray>();

        s_RenderBezierCurveData.BezierVertexBuffer = CreateRef<OpenGLVertexBuffer>(s_RenderBezierCurveData.MaxPoints * sizeof(Vertex));
        s_RenderBezierCurveData.BezierVertexBuffer->SetLayout({
            { ShaderDataType::Float3, "a_Position" },
            });

        s_RenderBezierCurveData.BezierVertexArray->AddVertexBuffer(s_RenderBezierCurveData.BezierVertexBuffer);

        s_RenderBezierCurveData.QuadraticBezierShader = s_ShaderLibrary->Get("QuadraticBezierCurveShader");
        s_RenderBezierCurveData.CubicBezierShader = s_ShaderLibrary->Get("CubicBezierCurveShader");
    }

    void Renderer::InitBezierPatchRenderData()
    {
        s_RenderBezierPatchData.BezierPatchVertexArray = CreateRef<OpenGLVertexArray>();

        s_RenderBezierPatchData.BezierPatchVertexBuffer = CreateRef<OpenGLVertexBuffer>(s_RenderBezierPatchData.PointCount * sizeof(Vertex));
        s_RenderBezierPatchData.BezierPatchVertexBuffer->SetLayout({
            { ShaderDataType::Float3, "a_Position" },
            });

        s_RenderBezierPatchData.BezierPatchVertexArray->AddVertexBuffer(s_RenderBezierPatchData.BezierPatchVertexBuffer);

        s_RenderBezierPatchData.BezierPatchShader = s_ShaderLibrary->Get("BezierPatchShader");

    }

    void Renderer::InitBSplinePatchRenderData()
    {
        s_RenderBSplinePatchData.BSplinePatchVertexArray = CreateRef<OpenGLVertexArray>();

        s_RenderBSplinePatchData.BSplinePatchVertexBuffer = CreateRef<OpenGLVertexBuffer>(s_RenderBSplinePatchData.PointCount * sizeof(Vertex));
        s_RenderBSplinePatchData.BSplinePatchVertexBuffer->SetLayout({
            { ShaderDataType::Float3, "a_Position" },
            });

        s_RenderBSplinePatchData.BSplinePatchVertexArray->AddVertexBuffer(s_RenderBSplinePatchData.BSplinePatchVertexBuffer);

        s_RenderBSplinePatchData.BSplinePatchShader = s_ShaderLibrary->Get("BSplinePatchShader");
    }

    void Renderer::InitSelectionBoxRenderData()
    {
        s_RenderSelectionBoxData.BoxVertexArray = CreateRef<OpenGLVertexArray>();

        s_RenderSelectionBoxData.BoxVertexBuffer = CreateRef<OpenGLVertexBuffer>(s_RenderSelectionBoxData.PointCount * 2 * sizeof(float));
        s_RenderSelectionBoxData.BoxVertexBuffer->SetLayout({
            { ShaderDataType::Float2, "a_Position" },
            });

        uint32_t indices[] =
        {
            0,1,2,
            2,3,0
        };

        s_RenderSelectionBoxData.BoxIndexBuffer = CreateRef<OpenGLIndexBuffer>(indices, 6);

        s_RenderSelectionBoxData.BoxVertexArray->AddVertexBuffer(s_RenderSelectionBoxData.BoxVertexBuffer);
        s_RenderSelectionBoxData.BoxVertexArray->SetIndexBuffer(s_RenderSelectionBoxData.BoxIndexBuffer);

        s_RenderSelectionBoxData.BoxShader = s_ShaderLibrary->Get("SelectionBoxShader");
    }

    void Renderer::ShutDown()
    {
    }

    void Renderer::OnWindowResize(uint32_t width, uint32_t height)
    {
        glViewport(0, 0, width, height);
    }

    void Renderer::BeginScene(const glm::mat4& viewProjectionMatrix)
    {
        s_SceneData->ViewProjectionMatrix = viewProjectionMatrix;

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
        if (!ShowPoints)
            return;

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

    void Renderer::RenderScreenQuad(const glm::vec2& bottomLeft, const glm::vec2& topRight, const glm::vec4& color)
    {
        glDisable(GL_DEPTH_TEST);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        float vertices[] =
        {
            bottomLeft.x,bottomLeft.y,
            topRight.x,bottomLeft.y,
            topRight.x,topRight.y,
            bottomLeft.x,topRight.y
        };

        s_RenderSelectionBoxData.BoxVertexArray->Bind();
        s_RenderSelectionBoxData.BoxVertexBuffer->SetData(vertices, sizeof(vertices));
        s_RenderSelectionBoxData.BoxShader->Bind();
        s_RenderSelectionBoxData.BoxShader->SetFloat4("u_Color", color);

        glDrawElements(GL_TRIANGLES, s_RenderSelectionBoxData.BoxIndexBuffer->GetCount(), GL_UNSIGNED_INT, 0);

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glEnable(GL_DEPTH_TEST);

    }

    void Renderer::RenderScreenQuadBorder(const glm::vec2& bottomLeft, const glm::vec2& topRight, const glm::vec4& color)
    {
        glDisable(GL_DEPTH_TEST);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        float vertices[] =
        {
            bottomLeft.x,bottomLeft.y,
            topRight.x,bottomLeft.y,
            topRight.x,topRight.y,
            bottomLeft.x,topRight.y
        };

        s_RenderSelectionBoxData.BoxVertexArray->Bind();
        s_RenderSelectionBoxData.BoxVertexBuffer->SetData(vertices, sizeof(vertices));
        s_RenderSelectionBoxData.BoxShader->Bind();
        s_RenderSelectionBoxData.BoxShader->SetFloat4("u_Color", color);

        glDrawElements(GL_TRIANGLES, s_RenderSelectionBoxData.BoxIndexBuffer->GetCount(), GL_UNSIGNED_INT, 0);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glEnable(GL_DEPTH_TEST);

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

    void Renderer::ShaderRenderBezierC0(const std::vector<glm::vec3>& controlPoints, const glm::vec4& color)
    {
        for (int i = 0; i < controlPoints.size(); i += 3)
        {
            if (i + 3 < controlPoints.size())
            {
                Renderer::ShaderRenderBezierC0(
                    controlPoints[i],
                    controlPoints[i + 1],
                    controlPoints[i + 2],
                    controlPoints[i + 3],
                    color);
            }
            else if (i + 2 < controlPoints.size())
            {
                Renderer::ShaderRenderBezierC0(
                    controlPoints[i],
                    controlPoints[i + 1],
                    controlPoints[i + 2],
                    color);
            }
            else if (i + 1 < controlPoints.size())
            {
                Renderer::RenderLine(controlPoints[i], controlPoints[i + 1], color);
            }
        }
    }


    void Renderer::ShaderRenderBezierC0(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, const glm::vec4& color)
    {
        float vertices[] = { p0.x,p0.y,p0.z,p1.x,p1.y,p1.z,p2.x,p2.y,p2.z };

        s_RenderBezierCurveData.BezierVertexArray->Bind();
        s_RenderBezierCurveData.BezierVertexBuffer->SetData(vertices, sizeof(float) * 9);
        s_RenderBezierCurveData.QuadraticBezierShader->Bind();
        s_RenderBezierCurveData.QuadraticBezierShader->SetFloat4("u_Color", color);
        s_RenderBezierCurveData.QuadraticBezierShader->SetMat4("u_ViewProjectionMatrix", s_SceneData->ViewProjectionMatrix);

        glDrawArrays(GL_TRIANGLES, 0, 3);

    }

    void Renderer::ShaderRenderBezierC0(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, const glm::vec4& color)
    {
        float vertices[] = { p0.x,p0.y,p0.z,p1.x,p1.y,p1.z,p2.x,p2.y,p2.z,p3.x,p3.y,p3.z };

        s_RenderBezierCurveData.BezierVertexArray->Bind();
        s_RenderBezierCurveData.BezierVertexBuffer->SetData(vertices, sizeof(float) * 12);
        s_RenderBezierCurveData.CubicBezierShader->Bind();
        s_RenderBezierCurveData.CubicBezierShader->SetFloat4("u_Color", color);
        s_RenderBezierCurveData.CubicBezierShader->SetMat4("u_ViewProjectionMatrix", s_SceneData->ViewProjectionMatrix);

        glDrawArrays(GL_LINES_ADJACENCY, 0, 4);
    }

    void Renderer::RenderBezierPatch(
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
        float vSubdivisionCount,
        const glm::vec4& color)
    {
        float vertices[] =
        {
            p0.x,p0.y,p0.z,
            p1.x,p1.y,p1.z,
            p2.x,p2.y,p2.z,
            p3.x,p3.y,p3.z,
            p4.x,p4.y,p4.z,
            p5.x,p5.y,p5.z,
            p6.x,p6.y,p6.z,
            p7.x,p7.y,p7.z,
            p8.x,p8.y,p8.z,
            p9.x,p9.y,p9.z,
            p10.x,p10.y,p10.z,
            p11.x,p11.y,p11.z,
            p12.x,p12.y,p12.z,
            p13.x,p13.y,p13.z,
            p14.x,p14.y,p14.z,
            p15.x,p15.y,p15.z,

            p0.x,p0.y,p0.z,
            p4.x,p4.y,p4.z,
            p8.x,p8.y,p8.z,
            p12.x,p12.y,p12.z,
            p1.x,p1.y,p1.z,
            p5.x,p5.y,p5.z,
            p9.x,p9.y,p9.z,
            p13.x,p13.y,p13.z,
            p2.x,p2.y,p2.z,
            p6.x,p6.y,p6.z,
            p10.x,p10.y,p10.z,
            p14.x,p14.y,p14.z,
            p3.x,p3.y,p3.z,
            p7.x,p7.y,p7.z,
            p11.x,p11.y,p11.z,
            p15.x,p15.y,p15.z,
        };

        s_RenderBezierPatchData.BezierPatchVertexArray->Bind();
        s_RenderBezierPatchData.BezierPatchVertexBuffer->SetData(vertices, sizeof(vertices));
        s_RenderBezierPatchData.BezierPatchShader->Bind();
        s_RenderBezierPatchData.BezierPatchShader->SetMat4("u_ViewProjectionMatrix", s_SceneData->ViewProjectionMatrix);
        s_RenderBezierPatchData.BezierPatchShader->SetFloat4("u_Color", color);
        s_RenderBezierPatchData.BezierPatchShader->SetFloat("u_SubdivisionCount", uSubdivisionCount);

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glPatchParameteri(GL_PATCH_VERTICES, 16);
        glDrawArrays(GL_PATCHES, 0, 16);

        s_RenderBezierPatchData.BezierPatchShader->SetFloat("u_SubdivisionCount", vSubdivisionCount);
        glDrawArrays(GL_PATCHES, 16, 16);

        glm::vec3 pp0 = p3;
        glm::vec3 pp1 = p7;
        glm::vec3 pp2 = p11;
        glm::vec3 pp3 = p15;

        ShaderRenderBezierC0(pp0, pp1, pp2, pp3, color);

        pp0 = p12;
        pp1 = p13;
        pp2 = p14;
        pp3 = p15;

        ShaderRenderBezierC0(pp0, pp1, pp2, pp3, color);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    void Renderer::RenderBSplinePatch(
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
        const glm::vec4& color)
    {
        float vertices[] =
        {
            p0.x,p0.y,p0.z,
            p1.x,p1.y,p1.z,
            p2.x,p2.y,p2.z,
            p3.x,p3.y,p3.z,
            p4.x,p4.y,p4.z,
            p5.x,p5.y,p5.z,
            p6.x,p6.y,p6.z,
            p7.x,p7.y,p7.z,
            p8.x,p8.y,p8.z,
            p9.x,p9.y,p9.z,
            p10.x,p10.y,p10.z,
            p11.x,p11.y,p11.z,
            p12.x,p12.y,p12.z,
            p13.x,p13.y,p13.z,
            p14.x,p14.y,p14.z,
            p15.x,p15.y,p15.z,

            p0.x,p0.y,p0.z,
            p4.x,p4.y,p4.z,
            p8.x,p8.y,p8.z,
            p12.x,p12.y,p12.z,
            p1.x,p1.y,p1.z,
            p5.x,p5.y,p5.z,
            p9.x,p9.y,p9.z,
            p13.x,p13.y,p13.z,
            p2.x,p2.y,p2.z,
            p6.x,p6.y,p6.z,
            p10.x,p10.y,p10.z,
            p14.x,p14.y,p14.z,
            p3.x,p3.y,p3.z,
            p7.x,p7.y,p7.z,
            p11.x,p11.y,p11.z,
            p15.x,p15.y,p15.z,
        };

        s_RenderBSplinePatchData.BSplinePatchVertexArray->Bind();
        s_RenderBSplinePatchData.BSplinePatchVertexBuffer->SetData(vertices, sizeof(vertices));
        s_RenderBSplinePatchData.BSplinePatchShader->Bind();
        s_RenderBSplinePatchData.BSplinePatchShader->SetMat4("u_ViewProjectionMatrix", s_SceneData->ViewProjectionMatrix);
        s_RenderBSplinePatchData.BSplinePatchShader->SetFloat4("u_Color", color);
        s_RenderBSplinePatchData.BSplinePatchShader->SetFloat("u_SubdivisionCount", uSubdivisionCount);

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glPatchParameteri(GL_PATCH_VERTICES, 16);
        glDrawArrays(GL_PATCHES, 0, 16);

        s_RenderBSplinePatchData.BSplinePatchShader->SetFloat("u_SubdivisionCount", vSubdivionCount);
        glDrawArrays(GL_PATCHES, 16, 16);

        glm::vec4 basisV = SplineBasis(1.0f);

        glm::vec3 pp0 = (basisV.x * p0 + basisV.y * p1 + basisV.z * p2 + basisV.w * p3);
        glm::vec3 pp1 = (basisV.x * p4 + basisV.y * p5 + basisV.z * p6 + basisV.w * p7);
        glm::vec3 pp2 = (basisV.x * p8 + basisV.y * p9 + basisV.z * p10 + basisV.w * p11);
        glm::vec3 pp3 = (basisV.x * p12 + basisV.y * p13 + basisV.z * p14 + basisV.w * p15);

        RenderBSpline(pp0, pp1, pp2, pp3, color);

        pp0 = (basisV.x * p0 + basisV.y * p4 + basisV.z * p8 + basisV.w * p12);
        pp1 = (basisV.x * p1 + basisV.y * p5 + basisV.z * p9 + basisV.w * p13);
        pp2 = (basisV.x * p2 + basisV.y * p6 + basisV.z * p10 + basisV.w * p14);
        pp3 = (basisV.x * p3 + basisV.y * p7 + basisV.z * p11 + basisV.w * p15);

        RenderBSpline(pp0, pp1, pp2, pp3, color);


        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    void Renderer::RenderBSpline(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, const glm::vec4& color)
    {
        auto midPoint0 = p0 * 2.0f / 3.0f + p1 * 1.0f / 3.0f;
        auto midPoint1 = p0 * 1.0f / 3.0f + p1 * 2.0f / 3.0f;
        auto midPoint2 = p1 * 2.0f / 3.0f + p2 * 1.0f / 3.0f;
        auto midPoint3 = p1 * 1.0f / 3.0f + p2 * 2.0f / 3.0f;
        auto midPoint4 = p2 * 2.0f / 3.0f + p3 * 1.0f / 3.0f;

        glm::vec3 pp0 = (midPoint1 + midPoint2) / 2.0f;
        glm::vec3 pp1 = midPoint2;
        glm::vec3 pp2 = midPoint3;
        glm::vec3 pp3 = (midPoint3 + midPoint4) / 2.0f;

        ShaderRenderBezierC0(pp0, pp1, pp2, pp3, color);
    }

    float CADMageddon::Renderer::Spline(float t, float ti, float interval)
    {
        if (ti > t && ti - 1.0f <= t)
            return 1;
        else
            return 0;
    }

    float CADMageddon::Renderer::Spline1(float t, float ti, float interval)
    {
        float val1 = Spline(t, ti) * (t - ti + interval);
        float val2 = Spline(t, ti + interval) * (ti + interval - t);
        return (val1 + val2) / interval;
    }

    float CADMageddon::Renderer::Spline2(float t, float ti, float interval)
    {
        float val1 = Spline1(t, ti) * (t - ti + interval);
        float val2 = Spline1(t, ti + interval) * (ti + 2 * interval - t);
        return (val1 + val2) / (2 * interval);
    }

    float CADMageddon::Renderer::Spline3(float t, float ti, float interval)
    {
        float val1 = Spline2(t, ti) * (t - ti + interval);
        float val2 = Spline2(t, ti + interval) * (ti + 3 * interval - t);
        return (val1 + val2) / (3 * interval);
    }

    glm::vec4 CADMageddon::Renderer::SplineBasis(float t)
    {
        float interval = 1.0f;
        return glm::vec4(Spline3(t, 1 - 3 * interval), Spline3(t, 1 - 2 * interval),
            Spline3(t, 1 - interval), Spline3(t, 1));
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
