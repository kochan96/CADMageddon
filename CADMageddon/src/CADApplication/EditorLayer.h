#pragma once
#include "Core\Layer.h"
#include "Rendering\VertexArray.h"
#include "Rendering\Shader.h"

namespace CADMageddon
{
    class EditorLayer : public Layer
    {
    public:
        EditorLayer(const std::string& debugName) :Layer(debugName) {}
        void OnAttach() override;
        void OnDetach() override;
        void OnUpdate(Timestep ts) override;
        void OnEvent(Event& event) override;

    private:
        Ref<OpenGLVertexBuffer> m_VertexBuffer;
        Ref<OpenGLIndexBuffer> m_IndexBuffer;
        Scope<OpenGLVertexArray> m_VertexArray;
        Scope<OpenGLShader> m_Shader;
    };
}