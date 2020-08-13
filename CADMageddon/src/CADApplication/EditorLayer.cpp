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
#include "Core/Input.h"

#include "Scene/ObjectFactory.h"
#include "Scene\Components.h"

#include "Gizmos\Gizmo.h"

namespace CADMageddon
{
    EditorLayer::EditorLayer(const std::string& debugName) :
        Layer(debugName), m_CameraController(45.0f, 1280.0f / 720.0f, 0.1f, 100.0f), m_CursorController(m_CameraController.GetCamera())
    {
        m_Viewport = std::make_pair<glm::vec2, glm::vec2>(glm::vec2(0.0f), glm::vec2(0.0f));
        m_Scene = CreateRef<Scene>();
        m_TransformationSystem = CreateRef<TransformationSystem>(m_CursorController.getCursor());
        m_PickingSystem = CreateRef<PickingSystem>(m_TransformationSystem);
        m_HierarchyPanel = CreateRef<HierarchyPanel>(m_Scene);

        m_HierarchyPanel->SetOnSelectionChangedCallback(std::bind(&EditorLayer::OnSelectionChanged, this, std::placeholders::_1, std::placeholders::_2));
        m_HierarchyPanel->SetOnSelectionClearedCallback(std::bind(&EditorLayer::OnSelectionCleared, this, std::placeholders::_1));

        m_InspectorPanel = CreateRef<InspectorPanel>();
    }

    void EditorLayer::OnAttach()
    {
        //glClearColor(0.39f, 0.58f, 0.93f, 1.0f);
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        //glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glEnable(GL_DEPTH_TEST);
        //glEnable(GL_MULTISAMPLE);


        FramebufferSpecification fbSpec;
        fbSpec.Width = 1280;
        fbSpec.Height = 720;
        m_Framebuffer = CreateRef<OpenGLFramebuffer>(fbSpec);

        InitImGui();
        InitGridVertexArray();

        std::srand(0);
    }

    bool EditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent& e)
    {

        if (!m_BlockEvents && m_EditorMode == EditorMode::Selection && e.GetMouseButton() == MouseCode::ButtonLeft)
        {
            auto pos = ImGui::GetIO().MousePos;
            auto viewPortMousePosition = GetViewPortMousePosition(glm::vec2(pos.x, pos.y));
            m_PickingSystem->Update(viewPortMousePosition, m_ViewportSize, *(m_Scene.get()), m_CameraController.GetCamera());
        }

        return false;
    }

    bool EditorLayer::OnKeyPressedEventViewport(KeyPressedEvent& e)
    {
        if (e.GetKeyCode() == CDM_KEY_SPACE)
        {
            m_Scene->CreatePointEntity(m_CursorController.getCursor()->getPosition());
        }

        if (e.GetKeyCode() == CDM_KEY_DELETE)
        {
            m_TransformationSystem->ClearSelection();
            m_InspectorPanel->Clear();
            m_Scene->DestroySelected();
        }

        return false;
    }

    bool EditorLayer::OnKeyPressedEvent(KeyPressedEvent& e)
    {
        if (e.GetKeyCode() == CDM_KEY_DELETE)
        {
            m_TransformationSystem->ClearSelection();
            m_InspectorPanel->Clear();
            m_Scene->DestroySelected();
        }

        return false;
    }

    void EditorLayer::OnSelectionChanged(bool selected, Entity entity)
    {
        if (selected)
        {
            m_TransformationSystem->AddToSelected(entity);
            m_InspectorPanel->Add(entity);
        }
        else
        {
            m_TransformationSystem->RemoveFromSelected(entity);
            m_InspectorPanel->Remove(entity);
        }
    }

    void EditorLayer::OnSelectionCleared(std::vector<Entity> cleared)
    {
        m_TransformationSystem->ClearSelection();
        m_InspectorPanel->Clear();
    }

    void EditorLayer::InitImGui()
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
        //TODO: make viewports work with glfwEvents
        //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows 

        //io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
        //io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

        // Setup Dear ImGui style
        //ImGui::StyleColorsDark();
        //ImGui::StyleColorsClassic();
        // ImGui Colors
        ImVec4* colors = ImGui::GetStyle().Colors;
        colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        colors[ImGuiCol_TextDisabled] = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
        colors[ImGuiCol_WindowBg] = ImVec4(0.18f, 0.18f, 0.18f, 1.0f); // Window background
        colors[ImGuiCol_ChildBg] = ImVec4(1.0f, 1.0f, 1.0f, 0.0f);
        colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
        colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.5f);
        colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
        colors[ImGuiCol_FrameBg] = ImVec4(0.3f, 0.3f, 0.3f, 0.5f); // Widget backgrounds
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.4f, 0.4f, 0.4f, 0.4f);
        colors[ImGuiCol_FrameBgActive] = ImVec4(0.4f, 0.4f, 0.4f, 0.6f);
        colors[ImGuiCol_TitleBg] = ImVec4(0.04f, 0.04f, 0.04f, 1.0f);
        colors[ImGuiCol_TitleBgActive] = ImVec4(0.29f, 0.29f, 0.29f, 1.0f);
        colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.0f, 0.0f, 0.0f, 0.51f);
        colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.0f);
        colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
        colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.0f);
        colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.0f);
        colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.0f);
        colors[ImGuiCol_CheckMark] = ImVec4(0.94f, 0.94f, 0.94f, 1.0f);
        colors[ImGuiCol_SliderGrab] = ImVec4(0.51f, 0.51f, 0.51f, 0.7f);
        colors[ImGuiCol_SliderGrabActive] = ImVec4(0.66f, 0.66f, 0.66f, 1.0f);
        colors[ImGuiCol_Button] = ImVec4(0.44f, 0.44f, 0.44f, 0.4f);
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.46f, 0.47f, 0.48f, 1.0f);
        colors[ImGuiCol_ButtonActive] = ImVec4(0.42f, 0.42f, 0.42f, 1.0f);
        colors[ImGuiCol_Header] = ImVec4(0.7f, 0.7f, 0.7f, 0.31f);
        colors[ImGuiCol_HeaderHovered] = ImVec4(0.7f, 0.7f, 0.7f, 0.8f);
        colors[ImGuiCol_HeaderActive] = ImVec4(0.48f, 0.5f, 0.52f, 1.0f);
        colors[ImGuiCol_Separator] = ImVec4(0.43f, 0.43f, 0.5f, 0.5f);
        colors[ImGuiCol_SeparatorHovered] = ImVec4(0.72f, 0.72f, 0.72f, 0.78f);
        colors[ImGuiCol_SeparatorActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.0f);
        colors[ImGuiCol_ResizeGrip] = ImVec4(0.91f, 0.91f, 0.91f, 0.25f);
        colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.81f, 0.81f, 0.81f, 0.67f);
        colors[ImGuiCol_ResizeGripActive] = ImVec4(0.46f, 0.46f, 0.46f, 0.95f);
        colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.0f);
        colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.0f, 0.43f, 0.35f, 1.0f);
        colors[ImGuiCol_PlotHistogram] = ImVec4(0.73f, 0.6f, 0.15f, 1.0f);
        colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.0f, 0.6f, 0.0f, 1.0f);
        colors[ImGuiCol_TextSelectedBg] = ImVec4(0.87f, 0.87f, 0.87f, 0.35f);
        colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.8f, 0.8f, 0.8f, 0.35f);
        colors[ImGuiCol_DragDropTarget] = ImVec4(1.0f, 1.0f, 0.0f, 0.9f);
        colors[ImGuiCol_NavHighlight] = ImVec4(0.60f, 0.6f, 0.6f, 1.0f);
        colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 1.0f, 1.0f, 0.7f);



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
            m_CursorController.Resize(m_ViewportSize, m_Viewport, m_CameraController.GetCamera());
        }


        if (m_ViewportFocused)
        {
            m_CameraController.OnUpdate(ts);
            m_CursorController.UpdateWorldPosition(m_CursorController.getCursor()->getPosition());
        }

        if (!m_BlockEvents && Input::IsMouseButtonPressed(CDM_MOUSE_BUTTON_LEFT) && m_EditorMode == EditorMode::MoveCursor)
        {
            auto pos = ImGui::GetIO().MousePos;
            auto viewPortMousePosition = GetViewPortMousePosition(glm::vec2(pos.x, pos.y));

            m_CursorController.Update(ts, m_CameraController.GetCamera(), viewPortMousePosition);
        }

        m_Framebuffer->Bind();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Renderer::BeginScene(m_CameraController.GetCamera());

        m_Scene->OnUpdate(ts);

        Renderer::RenderGrid(m_GridVertexArray, glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)), glm::vec4(1.0f));
        const float cursorSize = 1.0f;
        RenderCursor(m_CursorController.getCursor()->getPosition(), cursorSize);

        if (IsEditMode())
        {
            auto pos = ImGui::GetIO().MousePos;
            m_TransformationSystem->Update(m_Scene, m_CameraController.GetCamera(), GetNDCMousePosition({ pos.x,pos.y }));
        }

        Renderer::EndScene();

        m_Framebuffer->UnBind();

        RenderImGui();
    }

    bool EditorLayer::IsEditMode() const
    {
        return m_EditorMode == EditorMode::Translation || m_EditorMode == EditorMode::Rotation || m_EditorMode == EditorMode::Scale;
    }

    void EditorLayer::OnEvent(Event& event)
    {
        if (m_BlockEvents)
        {
            ImGuiIO& io = ImGui::GetIO();
            event.Handled |= event.IsInCategory(EventCategoryMouse) & io.WantCaptureMouse;
            event.Handled |= event.IsInCategory(EventCategoryKeyboard) & io.WantCaptureKeyboard;
            EventDispatcher dispatcher(event);
            dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(EditorLayer::OnKeyPressedEvent));
        }
        else
        {
            m_CameraController.OnEvent(event);
            EventDispatcher dispatcher(event);
            dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(EditorLayer::OnKeyPressedEventViewport));
            dispatcher.Dispatch<MouseButtonPressedEvent>(BIND_EVENT_FN(EditorLayer::OnMouseButtonPressed));
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
        m_HierarchyPanel->Render();
        m_InspectorPanel->Render();
        RenderViewport();
        RenderOptions();

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
                    m_Scene.reset(new Scene());
                    m_HierarchyPanel->SetScene(m_Scene);
                    m_TransformationSystem->ClearSelection();
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
                    m_Scene->CreateTorusEntity(m_CursorController.getCursor()->getPosition());
                }

                if (ImGui::MenuItem("Add Point"))
                {
                    m_Scene->CreatePointEntity(m_CursorController.getCursor()->getPosition());
                }

                if (ImGui::MenuItem("Add BezierC0"))
                {
                    m_Scene->CreateBezierC0Entity();
                }


                ImGui::EndMenu();
            }
        }

        ImGui::EndMainMenuBar();

    }

    void EditorLayer::RenderViewport()
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
        ImGui::Begin("Viewport");

        m_ViewportFocused = ImGui::IsWindowFocused();
        m_ViewportHovered = ImGui::IsWindowHovered();
        m_BlockEvents = !m_ViewportFocused || !m_ViewportHovered;

        {
            ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
            m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };
        }

        {
            ImVec2 vMin = ImGui::GetWindowContentRegionMin();
            ImVec2 vMax = ImGui::GetWindowContentRegionMax();

            vMin.x += ImGui::GetWindowPos().x;
            vMin.y += ImGui::GetWindowPos().y;
            vMax.x += ImGui::GetWindowPos().x;
            vMax.y += ImGui::GetWindowPos().y;

            m_Viewport.first = glm::vec2(vMin.x, vMin.y);
            m_Viewport.second = glm::vec2(vMax.x, vMax.y);

        }

        uint32_t textureID = m_Framebuffer->GetColorAttachmentRendererID();
        ImGui::Image((void*)textureID, ImVec2{ m_ViewportSize.x, m_ViewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
        ImGui::End();
        ImGui::PopStyleVar();


        ImGui::End();
    }

    void EditorLayer::RenderOptions()
    {
        ImGui::Begin("Options");

        ImGui::BeginGroup();

        ImGui::Text("Mode");

        if (ImGui::RadioButton("MoveCursor", m_EditorMode == EditorMode::MoveCursor))
        {
            m_EditorMode = EditorMode::MoveCursor;
        }

        if (ImGui::RadioButton("Selection", m_EditorMode == EditorMode::Selection))
        {
            m_EditorMode = EditorMode::Selection;
        }

        if (ImGui::RadioButton("Translation", m_EditorMode == EditorMode::Translation))
        {
            m_EditorMode = EditorMode::Translation;
            m_TransformationSystem->SetTransformationMode(TransformationMode::Translation);
        }

        if (ImGui::RadioButton("Rotation", m_EditorMode == EditorMode::Rotation))
        {
            m_EditorMode = EditorMode::Rotation;
            m_TransformationSystem->SetTransformationMode(TransformationMode::Rotation);
        }

        if (ImGui::RadioButton("Scale", m_EditorMode == EditorMode::Scale))
        {
            m_EditorMode = EditorMode::Scale;
            m_TransformationSystem->SetTransformationMode(TransformationMode::Scaling);
        }

        ImGui::EndGroup();

        ImGui::Separator();
        ImGui::BeginGroup();

        ImGui::Text("TransformationOrigin");

        if (ImGui::RadioButton("Center", m_TransformationSystem->GetTransformationOrigin() == TransformationOrigin::Center))
        {
            m_TransformationSystem->SetTransformationOrigin(TransformationOrigin::Center);
        }

        if (ImGui::RadioButton("Cursor", m_TransformationSystem->GetTransformationOrigin() == TransformationOrigin::Cursor))
        {
            m_TransformationSystem->SetTransformationOrigin(TransformationOrigin::Cursor);
        }

        ImGui::EndGroup();

        ImGui::Separator();

        ImGui::BeginGroup();


        auto cursor = m_CursorController.getCursor();
        auto m_WorldPosition = cursor->getPosition();
        auto m_ScreenPosition = cursor->getScreenPosition();
        ImGui::Text("Cursor");

        if (ImGui::DragFloat3("WorldPosition", &m_WorldPosition.x, 0.1f))
        {
            m_CursorController.UpdateWorldPosition(m_WorldPosition);
        }

        if (ImGui::DragFloat2("ScreenPosition", &m_ScreenPosition.x, 0.1f))
        {
            m_CursorController.UpdateScreenPosition(m_ScreenPosition);
        }

        ImGui::EndGroup();

        ImGui::End();
    }

    void EditorLayer::RenderCursor(glm::vec3 position, float cursorSize)
    {
        constexpr glm::vec4 redColor = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
        constexpr glm::vec4 greenColor = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
        constexpr glm::vec4 blueColor = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);

        auto startX = glm::vec3(position.x - cursorSize, position.y, position.z);
        auto endX = glm::vec3(position.x + cursorSize, position.y, position.z);

        auto startY = glm::vec3(position.x, position.y - cursorSize, position.z);
        auto endY = glm::vec3(position.x, position.y + cursorSize, position.z);

        auto startZ = glm::vec3(position.x, position.y, position.z - cursorSize);
        auto endZ = glm::vec3(position.x, position.y, position.z + cursorSize);


        Renderer::RenderLine(startX, endX, redColor);
        Renderer::RenderLine(startY, endY, greenColor);
        Renderer::RenderLine(startZ, endZ, blueColor);
    }

    glm::vec2 EditorLayer::GetNDCMousePosition(const glm::vec2& mousePosition)
    {
        glm::vec2 viewPortMousePosition = GetViewPortMousePosition(mousePosition);
        glm::vec2 ndc;

        ndc.x = (viewPortMousePosition.x * 2.0f) / m_ViewportSize.x - 1.0f;
        ndc.y = 1.0f - (viewPortMousePosition.y * 2.0f) / m_ViewportSize.y;

        return ndc;
    }

    glm::vec2 EditorLayer::GetViewPortMousePosition(const glm::vec2& mousePosition)
    {
        float x = mousePosition.x - m_Viewport.first.x;
        float y = mousePosition.y - m_Viewport.first.y;

        return glm::vec2(x, y);
    }

    bool EditorLayer::IsMouseInsideViewPort()
    {
        auto [x, y] = Input::GetMousePosition();

        if (x < m_Viewport.first.x || x > m_Viewport.second.x)
        {
            return false;
        }

        if (y < m_Viewport.first.y || y > m_Viewport.second.y)
        {
            return false;
        }

        return true;
    }
}
