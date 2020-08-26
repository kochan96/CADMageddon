#pragma once
#include "Core\Base.h"
#include "Shader.h"
#include "VertexArray.h"
#include <glm/glm.hpp>

namespace CADMageddon
{
    struct Vertex
    {
        glm::vec3 Position;
    };

    struct VertexC
    {
        glm::vec3 Position;
        glm::vec4 Color;
    };

    struct RenderTorusData
    {
        static const int MaxVertices = 20000;
        static const int MaxIndices = MaxVertices * 4;

        Ref<OpenGLVertexArray> TorusVertexArray;
        Ref<OpenGLVertexBuffer> TorusVertexBuffer;
        Ref<OpenGLIndexBuffer> TorusIndexBuffer;
        Ref<OpenGLShader> FlatColorShader;
    };

    struct RenderPointData
    {
        static const int MaxPoints = 20000;
        Ref <OpenGLVertexArray> PointsVertexArray;
        Ref<OpenGLVertexBuffer> PointsVertexBuffer;
        Ref<OpenGLShader> Shader;

        VertexC* PointVertexBufferBase = nullptr;
        VertexC* PointVertexBufferPtr = nullptr;

        int Count = 0;
    };

    struct RenderLineData
    {
        static const int MaxLines = 20000;
        Ref <OpenGLVertexArray> LinesVertexArray;
        Ref<OpenGLVertexBuffer> LinesVertexBuffer;
        Ref<OpenGLShader> Shader;

        VertexC* LinesVertexBufferBase = nullptr;
        VertexC* LinesVertexBufferPtr = nullptr;

        int Count = 0;
    };

    struct RenderBezierCurveData
    {
        static const int MaxPoints = 4;

        Ref<OpenGLVertexArray> BezierVertexArray;
        Ref<OpenGLVertexBuffer> BezierVertexBuffer;
        Ref<OpenGLShader> QuadraticBezierShader;
        Ref<OpenGLShader> CubicBezierShader;
    };

    struct RenderBezierPatchData
    {
        static const int PointCount = 32;
        Ref<OpenGLVertexArray> BezierPatchVertexArray;
        Ref<OpenGLVertexBuffer> BezierPatchVertexBuffer;
        Ref<OpenGLShader> BezierPatchShader;
    };

    struct RenderBSplinePatchData
    {
        static const int PointCount = 32;
        Ref<OpenGLVertexArray> BSplinePatchVertexArray;
        Ref<OpenGLVertexBuffer> BSplinePatchVertexBuffer;
        Ref<OpenGLShader> BSplinePatchShader;
    };

    struct RenderSelectionBoxData
    {
        static const int PointCount = 4;
        Ref<OpenGLVertexArray> BoxVertexArray;
        Ref<OpenGLVertexBuffer> BoxVertexBuffer;
        Ref<OpenGLIndexBuffer> BoxIndexBuffer;
        Ref<OpenGLShader> BoxShader;
    };
}