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

    struct RenderTorusData
    {
        static const int MaxVertices = 20000;
        static const int MaxIndices = MaxVertices * 4;

        Ref<OpenGLVertexArray> TorusVertexArray;
        Ref<OpenGLVertexBuffer> TorusVertexBuffer;
        Ref<OpenGLIndexBuffer> TorusIndexBuffer;
        Ref<OpenGLShader> FlatColorShader;
    };
}