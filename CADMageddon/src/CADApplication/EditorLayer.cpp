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

#include "ImguiEditors\ImGuiEditors.h"

namespace CADMageddon
{
    EditorLayer::EditorLayer(const std::string& debugName) :Layer(debugName), m_CameraController(45.0f, 1280.0f / 720.0f, 0.1f, 100.0f)
    {
    }

    void EditorLayer::OnAttach()
    {
        glClearColor(0.39f, 0.58f, 0.93f, 1.0f);
        glEnable(GL_DEPTH_TEST);
        //glEnable(GL_MULTISAMPLE);


        FramebufferSpecification fbSpec;
        fbSpec.Width = 1280;
        fbSpec.Height = 720;
        m_Framebuffer = CreateRef<OpenGLFramebuffer>(fbSpec);

        InitImGui();
        InitGridVertexArray();

        m_SelectedEntity = m_Scene.CreateTorusEntity("Torus");

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

    void EditorLayer::InitGridVertexArray()
    {
        m_GridVertexArray = CreateRef<OpenGLVertexArray>();
        auto gridMesh = ObjectFactory::CreateGridMesh(200.0f, 200.0f);

        auto vertexBuffer = CreateRef<OpenGLVertexBuffer>(&gridMesh.Vertices[0].x, gridMesh.Vertices.size() * 3 * sizeof(float));
        auto indexBuffer = CreateRef<OpenGLIndexBuffer>(gridMesh.Indices.data(), gridMesh.Indices.size());

        vertexBuffer->SetLayout({
            { ShaderDataType::Float3, "a_Position" }
            });

        m_GridVertexArray->AddVertexBuffer(vertexBuffer);
        m_GridVertexArray->SetIndexBuffer(indexBuffer);
    }

    void EditorLayer::OnDetach()
    {
        ShutDownImGui();
    }

    void EditorLayer::ShutDownImGui()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
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


        m_Framebuffer->Bind();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Renderer::BeginScene(m_CameraController.GetCamera());

        m_Scene.OnUpdate(ts);

        Renderer::RenderGrid(m_GridVertexArray, glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
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

        RenderMainMenuBar();
        RenderHierarchy();
        RenderInspector();
        RenderViewport();

        //ImGui::ShowDemoWindow();



        Application& app = Application::Get();
        io.DisplaySize = ImVec2((float)app.GetMainWindow().GetWidth(), (float)app.GetMainWindow().GetHeight());

        // Rendering
        ImGui::Render();
        //LOG_INFO(ImGui::GetIO().Framerate);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
    }

    void EditorLayer::RenderMainMenuBar()
    {
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("New"))
                {
                    //TODO
                }

                if (ImGui::MenuItem("Save"))
                {
                    //TODO
                }

                if (ImGui::MenuItem("Open"))
                {
                    //TODO
                }

                if (ImGui::MenuItem("Close"))
                {
                    Application::Get().Close();
                }

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Primitives"))
            {
                if (ImGui::MenuItem("Add Torus"))
                {
                    m_Scene.CreateTorusEntity("Torus");
                }

                ImGui::EndMenu();
            }
        }

        ImGui::EndMainMenuBar();

    }

    void EditorLayer::RenderHierarchy()
    {
        ImGui::Begin("Hierarchy");

        int id = 0;
        for (auto entity : m_Scene.GetEntities())
        {
            auto& nameComponent = entity.GetComponent<NameComponent>();

            std::string label = nameComponent.Name + "##" + std::to_string(id);
            if (ImGui::Selectable(label.c_str(), m_SelectedEntity == entity))
            {
                m_SelectedEntity = entity;
            }

            id++;
        }


        ImGui::End();
    }

    void EditorLayer::RenderInspector()
    {
        ImGui::Begin("Inspector");

        auto& nameComponent = m_SelectedEntity.GetComponent<NameComponent>();
        auto& torusComponent = m_SelectedEntity.GetComponent<TorusComponent>();
        auto& transformComponent = m_SelectedEntity.GetComponent<TransformComponent>();
        auto& colorComponent = m_SelectedEntity.GetComponent<ColorComponent>();

        NameComponentEditor(nameComponent);
        TransformComponentEditor(transformComponent);
        TorusComponentEditor(torusComponent);
        ColorComponentEditor(colorComponent);

        ImGui::End();
    }

    void EditorLayer::RenderViewport()
    {
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
    }
}
