#include "EditorLayer.h"
#include <glad\glad.h>

#include "Rendering\VertexArray.h"
#include "Rendering\Renderer.h"

#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include <imgui.h>
#include <examples/imgui_impl_glfw.h>
#include <examples/imgui_impl_opengl3.h>

#include "Core/Application.h"

#include "Scene/ObjectFactory.h"
#include "Scene\Components.h"

#include "ImguiEditors\TorusEditor.h"

namespace CADMageddon
{
    EditorLayer::EditorLayer(const std::string& debugName) :Layer(debugName), m_CameraController(45.0f, 1280.0f / 720.0f, 0.1f, 100.0f)
    {
    }

    void EditorLayer::OnAttach()
    {
        glClearColor(0.39f, 0.58f, 0.93f, 1.0f);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_MULTISAMPLE);


        m_VertexArray = CreateRef<OpenGLVertexArray>();
        m_Shader = CreateRef<OpenGLShader>("assets/shaders/FlatShader.glsl");

        m_Torus = m_Scene.CreateEntity();
        auto& torusComponent = m_Torus.AddComponent<TorusComponent>(1.0f, 0.3f, 30, 10);

        m_VertexBuffer = CreateRef<OpenGLVertexBuffer>(&torusComponent.Mesh.Vertices[0].x, torusComponent.Mesh.Vertices.size() * 3 * sizeof(float));
        m_VertexBuffer->SetLayout({
            { ShaderDataType::Float3, "a_Position" }
            });

        m_VertexArray->AddVertexBuffer(m_VertexBuffer);
        m_IndexBuffer = CreateRef<OpenGLIndexBuffer>(torusComponent.Mesh.Indices.data(), torusComponent.Mesh.Indices.size());

        m_VertexArray->SetIndexBuffer(m_IndexBuffer);
        FramebufferSpecification fbSpec;
        fbSpec.Width = 1280;
        fbSpec.Height = 720;
        m_Framebuffer = CreateRef<OpenGLFramebuffer>(fbSpec);

        InitImGui();



    }

    void EditorLayer::OnDetach()
    {
        ShutDownImGui();
    }

    void EditorLayer::OnUpdate(Timestep ts)
    {
        // Resize
        if (FramebufferSpecification spec = m_Framebuffer->GetSpecification();
            m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f && // zero sized framebuffer is invalid
            (spec.Width != m_ViewportSize.x || spec.Height != m_ViewportSize.y))
        {
            m_Framebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
            m_CameraController.OnResize(m_ViewportSize.x, m_ViewportSize.y);
        }


        if (m_ViewportFocused)
            m_CameraController.OnUpdate(ts);

        //TODO do not create opengl objects every frame

        auto& torusComponent = m_Torus.GetComponent<TorusComponent>();
        m_VertexArray = CreateRef<OpenGLVertexArray>();
        m_VertexBuffer = CreateRef<OpenGLVertexBuffer>(&torusComponent.Mesh.Vertices[0].x, torusComponent.Mesh.Vertices.size() * 3 * sizeof(float));
        m_VertexBuffer->SetLayout({
            { ShaderDataType::Float3, "a_Position" }
            });
        m_IndexBuffer = CreateRef<OpenGLIndexBuffer>(torusComponent.Mesh.Indices.data(), torusComponent.Mesh.Indices.size());
        
        m_VertexArray->AddVertexBuffer(m_VertexBuffer);
        m_VertexArray->SetIndexBuffer(m_IndexBuffer);

        m_Framebuffer->Bind();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Renderer::BeginScene(m_CameraController.GetCamera());
        Renderer::Submit(m_Shader, m_VertexArray, glm::mat4(1.0f), glm::vec4(0.3f, 0.2f, 0.1f, 1.0f));

        Renderer::EndScene();

        m_Framebuffer->UnBind();

        RenderImGui();
    }

    void EditorLayer::OnEvent(Event& event)
    {
        if (m_BlockEvents)
        {
            ImGuiIO& io = ImGui::GetIO();
            event.Handled |= event.IsInCategory(EventCategoryMouse) & io.WantCaptureMouse;
            event.Handled |= event.IsInCategory(EventCategoryKeyboard) & io.WantCaptureKeyboard;
        }
        else
        {
            m_CameraController.OnEvent(event);
            event.Handled = false;
        }
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
        GLFWwindow* window = static_cast<GLFWwindow*>(app.GetMainWindow().GetNativeWindow());

        // Setup Platform/Renderer bindings
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 410");
    }

    void EditorLayer::ShutDownImGui()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }


    void EditorLayer::RenderImGui()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        static bool dockspaceOpen = true;
        static bool opt_fullscreen_persistant = true;
        bool opt_fullscreen = opt_fullscreen_persistant;
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

        // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
        // because it would be confusing to have two docking targets within each others.
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        if (opt_fullscreen)
        {
            ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->Pos);
            ImGui::SetNextWindowSize(viewport->Size);
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
            window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        }

        // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            window_flags |= ImGuiWindowFlags_NoBackground;

        // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
        // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive, 
        // all active windows docked into it will lose their parent and become undocked.
        // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise 
        // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
        ImGui::PopStyleVar();

        if (opt_fullscreen)
            ImGui::PopStyleVar(2);

        // DockSpace
        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        }

        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                // Disabling fullscreen would allow the window to be moved to the front of other windows, 
                // which we can't undo at the moment without finer window depth/z control.
                //ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);

                if (ImGui::MenuItem("Exit")) Application::Get().Close();
                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }

        ImGui::Begin("Hierarchy");

        ImGui::Text("Label");

        ImGui::End();

        ImGui::Begin("Inspector");

        auto& torusComponent = m_Torus.GetComponent<TorusComponent>();
        TorusEditor(torusComponent);

        ImGui::End();

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
        ImGui::Begin("Viewport");

        m_ViewportFocused = ImGui::IsWindowFocused();
        m_ViewportHovered = ImGui::IsWindowHovered();
        m_BlockEvents = !m_ViewportFocused || !m_ViewportHovered;

        ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
        m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };

        uint32_t textureID = m_Framebuffer->GetColorAttachmentRendererID();
        ImGui::Image((void*)textureID, ImVec2{ m_ViewportSize.x, m_ViewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
        ImGui::End();
        ImGui::PopStyleVar();


        ImGui::End();

        Application& app = Application::Get();
        io.DisplaySize = ImVec2((float)app.GetMainWindow().GetWidth(), (float)app.GetMainWindow().GetHeight());

        // Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
    }
}
