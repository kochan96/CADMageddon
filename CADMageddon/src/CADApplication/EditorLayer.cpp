#include "EditorLayer.h"
#include <glad\glad.h>

#include "Rendering\VertexArray.h"

namespace CADMageddon
{
    void EditorLayer::OnAttach()
    {

        const char* vertexShaderSource = "#version 440 core\n"
            "layout (location = 0) in vec2 aPos;\n"
            "void main()\n"
            "{\n"
            "   gl_Position = vec4(aPos.x, aPos.y, 0.0f, 1.0);\n"
            "}\0";

        const char* fragmentShaderSource = "#version 440 core\n"
            "out vec4 FragColor;\n"
            "void main()\n"
            "{\n"
            "   FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);\n"
            "}\0";


        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        m_VertexArray = CreateScope<OpenGLVertexArray>();
        m_Shader = CreateScope<OpenGLShader>("Test", vertexShaderSource, fragmentShaderSource);

        float positions[8] =
        {
            -0.5f,-0.5f,
            0.5f,-0.5f,
            0.5f,0.5f,
            -0.5f,0.5f
        };

        uint32_t indices[6]
        {
            0,1,2,
            2,3,0
        };

        m_VertexBuffer = CreateRef<OpenGLVertexBuffer>(positions, sizeof(positions));
        m_VertexBuffer->SetLayout({
            { ShaderDataType::Float2, "aPos" }
            });

        m_VertexArray->AddVertexBuffer(m_VertexBuffer);
        m_IndexBuffer = CreateRef<OpenGLIndexBuffer>(indices, 6);

        m_VertexArray->SetIndexBuffer(m_IndexBuffer);

    }

    void EditorLayer::OnDetach()
    {
    }

    void EditorLayer::OnUpdate(Timestep ts)
    {
        glClear(GL_COLOR_BUFFER_BIT);

        m_VertexArray->Bind();
        m_Shader->Bind();

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }

    void EditorLayer::OnEvent(Event& event)
    {

    }
}
