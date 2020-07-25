#include "EditorLayer.h"
#include <glad\glad.h>

#include "Rendering\VertexArray.h"
#include "Rendering\Renderer.h"

#include <glm\gtc\matrix_transform.hpp>

#include <imgui.h>
#include <examples/imgui_impl_glfw.h>
#include <examples/imgui_impl_opengl3.h>

namespace CADMageddon
{
    EditorLayer::EditorLayer(const std::string& debugName) :Layer(debugName), m_CameraController(45.0f, 1280.0f / 720.0f, 0.1f, 100.0f)
    {
    }

    void EditorLayer::OnAttach()
    {
        glClearColor(0.39f, 0.58f, 0.93f, 1.0f);
        glEnable(GL_DEPTH_TEST);


        m_VertexArray = CreateRef<OpenGLVertexArray>();
        m_Shader = CreateRef<OpenGLShader>("assets/shaders/FlatShader.glsl");

        float positions[8 * 3] =
        {
            -0.5f,-0.5f,0.5f,
            0.5f,-0.5f,0.5f,
            0.5f,0.5f,0.5f,
            -0.5f,0.5f,0.5f,

            -0.5f,-0.5f,-0.5f,
            0.5f,-0.5f,-0.5f,
            0.5f,0.5f,-0.5f,
            -0.5f,0.5f,-0.5f,
        };

        uint32_t indices[36]
        {
            0,1,2,
            2,3,0, //front

            4,5,6,
            6,7,4, //back

            0,4,7,
            7,3,0, //left

            1,5,6,
            6,2,1, //right

            3,2,6,
            6,7,3, //top

            0,1,5,
            5,4,0, //bottom
        };

        m_VertexBuffer = CreateRef<OpenGLVertexBuffer>(positions, sizeof(positions));
        m_VertexBuffer->SetLayout({
            { ShaderDataType::Float3, "a_Position" }
            });

        m_VertexArray->AddVertexBuffer(m_VertexBuffer);
        m_IndexBuffer = CreateRef<OpenGLIndexBuffer>(indices, 36);

        m_VertexArray->SetIndexBuffer(m_IndexBuffer);

        m_Shader->Bind();

        m_Shader->SetFloat4("u_Color", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
    }

    void EditorLayer::OnDetach()
    {
    }

    void EditorLayer::OnUpdate(Timestep ts)
    {

        m_CameraController.OnUpdate(ts);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Renderer::BeginScene(m_CameraController.GetCamera());
        Renderer::Submit(m_Shader, m_VertexArray);
       
        Renderer::EndScene();
    }

    void EditorLayer::OnEvent(Event& event)
    {
        m_CameraController.OnEvent(event);
    }

    void EditorLayer::InitImGui()
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
        //io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
        //io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        //ImGui::StyleColorsClassic();

        // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
        ImGuiStyle& style = ImGui::GetStyle();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        Application& app = Application::Get();
        GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());

        // Setup Platform/Renderer bindings
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 410");
    }

    void EditorLayer::ShutDownImGui()
    {

    }
}
