#pragma once
#include "Core\Layer.h"
#include "Rendering\VertexArray.h"
#include "Rendering\Shader.h"

#include "Rendering\FPSCameraController.h"

namespace CADMageddon
{
    class EditorLayer : public Layer
    {
    public:
        EditorLayer(const std::string& debugName);
        void OnAttach() override;
        void OnDetach() override;
        void OnUpdate(Timestep ts) override;
        void OnEvent(Event& event) override;

    private:
        void InitImGui();
        void ShutDownImGui();

    private:

        Ref<OpenGLVertexBuffer> m_VertexBuffer;
        Ref<OpenGLIndexBuffer> m_IndexBuffer;
        Ref<OpenGLVertexArray> m_VertexArray;
        Ref<OpenGLShader> m_Shader;

        FPSCameraController m_CameraController;
    };
}