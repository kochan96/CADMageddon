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

    struct VertexT
    {
        glm::vec3 Position;
        glm::vec2 TextureCoordinates;
    };

    struct RenderTorusData
    {
        static const int MaxVertices = 20000;
        static const int MaxIndices = MaxVertices * 4;

        Ref<OpenGLVertexArray> TorusVertexArray;
        Ref<OpenGLVertexBuffer> TorusVertexBuffer;
        Ref<OpenGLIndexBuffer> TorusIndexBuffer;
        Ref<OpenGLShader> TorusShader;
    };

    struct RenderPointData
    {
        static const int MaxPoints = 50000;
        Ref <OpenGLVertexArray> PointsVertexArray;
        Ref<OpenGLVertexBuffer> PointsVertexBuffer;
        Ref<OpenGLShader> Shader;

        VertexC* PointVertexBufferBase = nullptr;
        VertexC* PointVertexBufferPtr = nullptr;

        int Count = 0;
    };

    struct RenderLineData
    {
        static const int MaxLines = 50000;
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
        static const int MaxPatches = 1000;
        static const int MaxPoints = 16 * MaxPatches;
        static const int MaxIndices = 64 * MaxPatches;
        Ref<OpenGLVertexArray> BezierPatchVertexArray;
        Ref<OpenGLIndexBuffer> BezierPatchIndexBuffer;
        Ref<OpenGLVertexBuffer> BezierPatchVertexBuffer;
        Ref<OpenGLShader> BezierPatchShader;
    };

    struct RenderBSplinePatchData
    {
        static const int MaxPatches = 1000;
        static const int MaxPoints = 16 * MaxPatches;
        static const int MaxIndices = 64 * MaxPatches;
        Ref<OpenGLVertexArray> BSplinePatchVertexArray;
        Ref<OpenGLVertexBuffer> BSplinePatchVertexBuffer;
        Ref<OpenGLIndexBuffer> BSplinePatchIndexBuffer;
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

    struct RenderGregoryPatchData
    {
        static const int MaxPoints = 200;
        Ref<OpenGLVertexArray> GregoryVertexArray;
        Ref<OpenGLVertexBuffer> GregoryVertexBuffer;
        Ref<OpenGLShader> GregoryShader;
    };

    struct RenderTextureQuadData
    {
        static const int PointCount = 6;
        Ref<OpenGLVertexArray> TextureQuadVertexArray;
        Ref<OpenGLVertexBuffer> TextureQuadVertexBuffer;
        Ref<OpenGLIndexBuffer> TextureQuadIndexBuffer;
        Ref<OpenGLShader> TextureQuadShader;
    };
}