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

#include "Gizmos\Gizmo.h"

#include "Serialization/tinyfiledialogs.h"
#include "Serialization\SceneSerializer.h"


namespace CADMageddon
{
    EditorLayer::EditorLayer(const std::string& debugName) :
        Layer(debugName), m_CameraController(45.0f, 1280.0f / 720.0f, 0.1f, 1000.0f), m_CursorController(m_CameraController.GetCamera())
    {
        m_Viewport = std::make_pair<glm::vec2, glm::vec2>(glm::vec2(0.0f), glm::vec2(0.0f));
        m_Scene = CreateRef<Scene>();
        m_TransformationSystem = CreateRef<TransformationSystem>(m_CursorController.getCursor());
        m_PickingSystem = CreateRef<PickingSystem>(m_TransformationSystem);
        m_HierarchyPanel = CreateRef<HierarchyPanel>(m_Scene);

        m_HierarchyPanel->SetOnSelectionChangedPointCallback(std::bind(&EditorLayer::OnSelectionChangedPoint, this, std::placeholders::_1, std::placeholders::_2));
        m_HierarchyPanel->SetOnSelectionChangedTorusCallback(std::bind(&EditorLayer::OnSelectionChangedTorus, this, std::placeholders::_1, std::placeholders::_2));
        m_HierarchyPanel->SetOnBezierSelectionChangedCallback(std::bind(&EditorLayer::OnSelectionChangedBezierC0, this, std::placeholders::_1, std::placeholders::_2));
        m_HierarchyPanel->SetOnBSplineSelectionChangedCallback(std::bind(&EditorLayer::OnSelectionChangedBSpline, this, std::placeholders::_1, std::placeholders::_2));
        m_HierarchyPanel->SetOnInterpolatedSelectionChangedCallback(std::bind(&EditorLayer::OnSelectionInterpolatedChanged, this, std::placeholders::_1, std::placeholders::_2));
        m_HierarchyPanel->SetOnBezierPatchSelectionChanged(std::bind(&EditorLayer::OnSelectionChangedBezierPatch, this, std::placeholders::_1, std::placeholders::_2));
        m_HierarchyPanel->SetOnBSplinePatchSelectionChanged(std::bind(&EditorLayer::OnSelectionChangedBSplinePatch, this, std::placeholders::_1, std::placeholders::_2));
        m_HierarchyPanel->SetOnGregoryPatchSelectionChanged(std::bind(&EditorLayer::OnSelectionChangedGregoryPatch, this, std::placeholders::_1, std::placeholders::_2));
        m_HierarchyPanel->SetOnSelectionClearedCallback(std::bind(&EditorLayer::OnSelectionCleared, this));

        m_InspectorPanel = CreateRef<InspectorPanel>(m_Scene, m_TransformationSystem);

        m_PickingSystem->SetOnPointSelectionChanged(std::bind(&EditorLayer::OnSelectionChangedPoint, this, std::placeholders::_1, std::placeholders::_2));
        m_PickingSystem->SetOnTorusSelectionChanged(std::bind(&EditorLayer::OnSelectionChangedTorus, this, std::placeholders::_1, std::placeholders::_2));
        m_PickingSystem->SetOnSelectionCleared(std::bind(&EditorLayer::OnPickingSelectionCleared, this));

        m_Scene->SetOnPointMerged(std::bind(&EditorLayer::OnPointsMergedCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        m_Scene->SetOnGregoryPatchDeleted(std::bind(&EditorLayer::OnGregoryDeletedCallback, this, std::placeholders::_1));
    }

    void EditorLayer::OnAttach()
    {
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glEnable(GL_DEPTH_TEST);


        FramebufferSpecification fbSpec;
        fbSpec.Width = 1280;
        fbSpec.Height = 720;
        m_Framebuffer = CreateRef<OpenGLFramebuffer>(fbSpec);
        m_FramebufferLeft = CreateRef<OpenGLFramebuffer>(fbSpec);
        m_FramebufferRight = CreateRef<OpenGLFramebuffer>(fbSpec);

        InitImGui();
        InitGridVertexArray();
        InitQuadVertexArray();
        InitQuadShader();

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
            m_Scene->CreatePoint(m_CursorController.getCursor()->getPosition(), "Point");
        }

        if (e.GetKeyCode() == CDM_KEY_DELETE)
        {
            m_TransformationSystem->ClearSelection();
            m_InspectorPanel->Clear();
            m_Scene->DeleteSelected();
        }

        return false;
    }

    bool EditorLayer::OnKeyPressedEvent(KeyPressedEvent& e)
    {
        if (e.GetKeyCode() == CDM_KEY_DELETE)
        {
            m_TransformationSystem->ClearSelection();
            m_InspectorPanel->Clear();
            m_Scene->DeleteSelected();
        }

        return false;
    }

    void EditorLayer::OnSelectionChangedPoint(bool selected, Ref<Point> point)
    {
        if (selected)
        {
            m_TransformationSystem->AddToSelected(point->GetTransform());
            m_InspectorPanel->AddPoint(point);
        }
        else
        {
            m_TransformationSystem->RemoveFromSelected(point->GetTransform());
            m_InspectorPanel->RemovePoint(point);
        }
    }

    void EditorLayer::OnSelectionChangedTorus(bool selected, Ref<Torus> torus)
    {
        if (selected)
        {
            m_TransformationSystem->AddToSelected(torus->GetTransform());
            m_InspectorPanel->AddTorus(torus);
        }
        else
        {
            m_TransformationSystem->RemoveFromSelected(torus->GetTransform());
            m_InspectorPanel->RemoveTorus(torus);
        }
    }

    void EditorLayer::OnSelectionChangedBezierC0(bool selected, Ref<BezierC0> bezier)
    {
        if (selected)
        {
            m_InspectorPanel->AddBezierC0(bezier);
        }
        else
        {
            m_InspectorPanel->RemoveBezierC0(bezier);
        }
    }

    void EditorLayer::OnSelectionChangedBSpline(bool selected, Ref<BSpline> bSpline)
    {
        if (selected)
        {
            m_InspectorPanel->AddBSpline(bSpline);
        }
        else
        {
            m_InspectorPanel->RemoveBSpline(bSpline);
        }
    }

    void EditorLayer::OnSelectionInterpolatedChanged(bool selected, Ref<InterpolatedCurve> interpolated)
    {
        if (selected)
        {
            m_InspectorPanel->AddInterpolatedCurve(interpolated);
        }
        else
        {
            m_InspectorPanel->RemoveInterpolatedCurve(interpolated);
        }
    }

    void EditorLayer::OnSelectionChangedBezierPatch(bool selected, Ref<BezierPatch> bezierPatch)
    {
        if (selected)
        {
            m_InspectorPanel->AddBezierPatch(bezierPatch);
        }
        else
        {
            m_InspectorPanel->RemoveBezierPatch(bezierPatch);
        }
    }

    void EditorLayer::OnSelectionChangedBSplinePatch(bool selected, Ref<BSplinePatch> bezierPatch)
    {
        if (selected)
        {
            m_InspectorPanel->AddBSplinePatch(bezierPatch);
        }
        else
        {
            m_InspectorPanel->RemoveBSplinePatch(bezierPatch);
        }
    }

    void EditorLayer::OnSelectionChangedGregoryPatch(bool selected, Ref<GregoryPatch> gregoryPatch)
    {
        if (selected)
        {
            m_InspectorPanel->AddGregoryPatch(gregoryPatch);
        }
        else
        {
            m_InspectorPanel->RemoveGregoryPatch(gregoryPatch);
        }
    }

    void EditorLayer::OnPointsMergedCallback(Ref<Point> p1, Ref<Point> p2, Ref<Point> newPoint)
    {
        m_InspectorPanel->RemovePoint(p1);
        m_InspectorPanel->RemovePoint(p2);

        m_TransformationSystem->RemoveFromSelected(p1->GetTransform());
        m_TransformationSystem->RemoveFromSelected(p2->GetTransform());
    }

    void EditorLayer::OnGregoryDeletedCallback(Ref<GregoryPatch> gregory)
    {
        m_InspectorPanel->RemoveGregoryPatch(gregory);
    }

    void EditorLayer::OnPickingSelectionCleared()
    {
        m_TransformationSystem->ClearSelection();
        m_InspectorPanel->ClearPointsAndToruses();
    }

    void EditorLayer::OnSelectionCleared()
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

    void EditorLayer::InitQuadVertexArray()
    {
        float quadVertices[24] = {
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
        };

        m_QuadVertexArray = CreateRef<OpenGLVertexArray>();
        auto vertexBuffer = CreateRef<OpenGLVertexBuffer>(quadVertices, sizeof(quadVertices));
        vertexBuffer->SetLayout({
            { ShaderDataType::Float2, "a_Position" },
            { ShaderDataType::Float2, "a_TextureCoord" }
            });

        m_QuadVertexArray->AddVertexBuffer(vertexBuffer);
    }

    void EditorLayer::InitQuadShader()
    {
        m_QuadShader = CreateRef<OpenGLShader>("assets/shaders/QuadShader.glsl");
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
            m_FramebufferLeft->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
            m_FramebufferRight->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
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

        if (!m_EnableStereoscopic)
        {
            m_Framebuffer->Bind();

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            Renderer::BeginScene(m_CameraController.GetCamera().GetViewProjectionMatrix());

            m_Scene->Update();
            if (m_ShowGrid)
                Renderer::RenderGrid(m_GridVertexArray, glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)), glm::vec4(1.0f));
            const float cursorSize = 1.0f;
            RenderCursor(m_CursorController.getCursor()->getPosition(), cursorSize);

            if (m_TransformationSystem->GetCount() > 1)
            {
                Renderer::RenderPoint(m_TransformationSystem->GetTransformationCenter(), glm::vec4(1.0f, 0.0f, 1.0f, 1.0f));
            }

            if (IsEditMode())
            {
                auto pos = ImGui::GetIO().MousePos;
                m_TransformationSystem->Update(m_Scene, m_CameraController.GetCamera(), GetNDCMousePosition({ pos.x,pos.y }));
            }

            if (!m_BlockEvents && m_EditorMode == EditorMode::Selection)
            {
                auto pos = ImGui::GetIO().MousePos;
                auto viewPortMousePosition = GetViewPortMousePosition(glm::vec2(pos.x, pos.y));
                m_PickingSystem->UpdateMultiSelect(viewPortMousePosition, m_ViewportSize, *(m_Scene.get()), m_CameraController.GetCamera());
            }

            Renderer::EndScene();

            m_Framebuffer->UnBind();
        }
        else
        {
            m_FramebufferLeft->Bind();
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            auto leftEyeMatrix = m_CameraController.GetCamera().GetLeftEyeProjectionMatrix() * m_CameraController.GetCamera().GetLeftViewMatrix();
            Renderer::BeginScene(leftEyeMatrix);


            m_Scene->SetDefaultColor(m_LeftEyeColor);
            m_Scene->SetSelectionColor(m_LeftEyeColor);
            m_Scene->Update();
            if (m_ShowGrid)
                Renderer::RenderGrid(m_GridVertexArray, glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)), m_LeftEyeColor);

            const float cursorSize = 1.0f;
            RenderCursor(m_CursorController.getCursor()->getPosition(), cursorSize, m_LeftEyeColor);

            if (IsEditMode())
            {
                auto pos = ImGui::GetIO().MousePos;
                m_TransformationSystem->Update(m_Scene, m_CameraController.GetCamera(), GetNDCMousePosition({ pos.x,pos.y }));
            }

            if (!m_BlockEvents && m_EditorMode == EditorMode::Selection)
            {
                auto pos = ImGui::GetIO().MousePos;
                auto viewPortMousePosition = GetViewPortMousePosition(glm::vec2(pos.x, pos.y));
                m_PickingSystem->UpdateMultiSelect(viewPortMousePosition, m_ViewportSize, *(m_Scene.get()), m_CameraController.GetCamera());
            }

            Renderer::EndScene();

            m_FramebufferRight->Bind();
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            auto rightEyeMatrix = m_CameraController.GetCamera().GetRightEyeProjectionMatrix() * m_CameraController.GetCamera().GetRightViewMatrix();
            Renderer::BeginScene(rightEyeMatrix);

            m_Scene->SetDefaultColor(m_RightEyeColor);
            m_Scene->SetSelectionColor(m_RightEyeColor);
            m_Scene->Update();

            if (m_ShowGrid)
                Renderer::RenderGrid(m_GridVertexArray, glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)), m_RightEyeColor);

            RenderCursor(m_CursorController.getCursor()->getPosition(), cursorSize, m_RightEyeColor);

            if (IsEditMode())
            {
                auto pos = ImGui::GetIO().MousePos;
                m_TransformationSystem->Update(m_Scene, m_CameraController.GetCamera(), GetNDCMousePosition({ pos.x,pos.y }));
            }

            Renderer::EndScene();

            m_Framebuffer->Bind();
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            m_QuadVertexArray->Bind();
            m_QuadShader->Bind();
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, m_FramebufferLeft->GetColorAttachmentRendererID());
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, m_FramebufferRight->GetColorAttachmentRendererID());

            m_QuadShader->SetInt("leftEyeFrame", 0);
            m_QuadShader->SetInt("rightEyeFrame", 1);
            //m_QuadShader->SetFloat3("leftFilter", m_LeftFilter);
            //m_QuadShader->SetFloat3("rightFilter", m_RightFilter);
            glDrawArrays(GL_TRIANGLES, 0, 6);

            m_Framebuffer->UnBind();

        }

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
        char const* lTheSaveFileName, * lTheOpenFileName;
        char const* lFilterPatterns[] = { "*.xml" };

        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("New"))
                {
                    m_Scene.reset(new Scene());
                    m_HierarchyPanel->SetScene(m_Scene);
                    m_InspectorPanel->SetScene(m_Scene);
                    m_TransformationSystem->ClearSelection();
                    m_InspectorPanel->Clear();
                }

                if (ImGui::MenuItem("Save"))
                {
                    auto lTheSaveFileName = tinyfd_saveFileDialog(
                        "Save your scene as",
                        "rybcia.xml",
                        1,
                        lFilterPatterns,
                        NULL);
                    if (lTheSaveFileName) {
                        SceneSerializer::SerializeScene(*m_Scene, lTheSaveFileName);
                    }
                }

                if (ImGui::MenuItem("Open"))
                {
                    lTheOpenFileName = tinyfd_openFileDialog(
                        "Choose an XML file to read",
                        "",
                        1,
                        lFilterPatterns,
                        NULL,
                        0);
                    if (lTheOpenFileName) {
                        m_Scene = SceneSerializer::LoadScene(lTheOpenFileName);
                        m_HierarchyPanel->SetScene(m_Scene);
                        m_InspectorPanel->SetScene(m_Scene);
                        m_TransformationSystem->ClearSelection();
                        m_InspectorPanel->Clear();
                    }
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
                    m_Scene->CreateTorus(m_CursorController.getCursor()->getPosition(), "Torus");
                }

                if (ImGui::MenuItem("Add Point"))
                {
                    m_Scene->CreatePoint(m_CursorController.getCursor()->getPosition(), "Point");
                }

                if (ImGui::MenuItem("Add BezierC0"))
                {
                    m_Scene->CreateBezierC0("BezierC0");
                }

                if (ImGui::MenuItem("Add BSpline"))
                {
                    m_Scene->CreateBSpline("BSpline");
                }

                if (ImGui::MenuItem("Add Interpolated"))
                {
                    m_Scene->CreateInterpolated("Interpolated");
                }

                if (ImGui::MenuItem("Add BezierPatch Rect"))
                {
                    m_ShowBezierPatchRectCreationPopup = true;
                }

                if (ImGui::MenuItem("Add BezierPatch Cylinder"))
                {
                    m_ShowBezierPatchCylinderCreationPopup = true;
                }

                if (ImGui::MenuItem("Add BSplinePatch Rect"))
                {
                    m_ShowBSplinePatchRectCreationPopup = true;
                }

                if (ImGui::MenuItem("Add BSplinePatch Cylinder"))
                {
                    m_ShowBSplinePatchCylinderCreationPopup = true;
                }

                if (m_ShowBezierPatchCylinderCreationPopup || m_ShowBSplinePatchCylinderCreationPopup)
                {
                    m_PatchCylinderCreationParameters.Center = m_CursorController.getCursor()->getPosition();
                    m_PatchCylinderCreationParameters.PatchCountX = m_ShowBSplinePatchCylinderCreationPopup ? 4 : 1;
                    m_PatchCylinderCreationParameters.PatchCountY = 1;
                    m_PatchCylinderCreationParameters.Radius = 4.0f;
                    m_PatchCylinderCreationParameters.Height = 4.0f;
                }

                if (m_ShowBezierPatchRectCreationPopup || m_ShowBSplinePatchRectCreationPopup)
                {
                    m_PatchRectCreationParameters.Position = m_CursorController.getCursor()->getPosition();
                    m_PatchRectCreationParameters.PatchCountX = 1;
                    m_PatchRectCreationParameters.PatchCountY = 1;
                    m_PatchRectCreationParameters.Width = 4.0f;
                    m_PatchRectCreationParameters.Height = 4.0f;
                }

                ImGui::EndMenu();
            }
        }

        ImGui::EndMainMenuBar();

        if (m_ShowBezierPatchRectCreationPopup)
        {
            ImGui::OpenPopup("BezierPatchRect Creation");
        }

        if (ImGui::BeginPopupModal("BezierPatchRect Creation", &m_ShowBezierPatchRectCreationPopup))
        {
            ImGui::DragInt("PatchCountX", &m_PatchRectCreationParameters.PatchCountX, 1.0f, 1);
            ImGui::DragInt("PatchCountY", &m_PatchRectCreationParameters.PatchCountY, 1.0f, 1);
            ImGui::DragFloat("Width", &m_PatchRectCreationParameters.Width, 0.1f);
            ImGui::DragFloat("Height", &m_PatchRectCreationParameters.Height, 0.1f);

            if (ImGui::Button("Cancel"))
            {
                m_ShowBezierPatchRectCreationPopup = false;
            }
            ImGui::SameLine();
            if (ImGui::Button("Create"))
            {
                m_Scene->CreateBezierPatchRect("BezierPatchRect", m_PatchRectCreationParameters);
                m_ShowBezierPatchRectCreationPopup = false;
            }

            ImGui::EndPopup();

        }

        if (m_ShowBezierPatchCylinderCreationPopup)
        {
            ImGui::OpenPopup("BezierPatchCylinder Creation");
        }

        if (ImGui::BeginPopupModal("BezierPatchCylinder Creation", &m_ShowBezierPatchCylinderCreationPopup))
        {
            int patchX = m_PatchCylinderCreationParameters.PatchCountX;
            if (ImGui::DragInt("PatchCountX", &patchX, 1.0f, 1))
            {
                m_PatchCylinderCreationParameters.PatchCountX = patchX;
            }
            ImGui::DragInt("PatchCountY", &m_PatchCylinderCreationParameters.PatchCountY, 1.0f, 1);
            ImGui::DragFloat("Radius", &m_PatchCylinderCreationParameters.Radius, 0.1f);
            ImGui::DragFloat("Height", &m_PatchCylinderCreationParameters.Height, 0.1f);

            if (ImGui::Button("Cancel"))
            {
                m_ShowBezierPatchCylinderCreationPopup = false;
            }
            ImGui::SameLine();
            if (ImGui::Button("Create"))
            {
                m_Scene->CreateBezierPatchCylinder("BezierPatchCylinder", m_PatchCylinderCreationParameters);
                m_ShowBezierPatchCylinderCreationPopup = false;
            }

            ImGui::EndPopup();

        }

        if (m_ShowBSplinePatchRectCreationPopup)
        {
            ImGui::OpenPopup("BSplinePatchRect Creation");
        }

        if (ImGui::BeginPopupModal("BSplinePatchRect Creation", &m_ShowBSplinePatchRectCreationPopup))
        {
            ImGui::DragInt("PatchCountX", &m_PatchRectCreationParameters.PatchCountX, 1.0f, 1);
            ImGui::DragInt("PatchCountY", &m_PatchRectCreationParameters.PatchCountY, 1.0f, 1);
            ImGui::DragFloat("Width", &m_PatchRectCreationParameters.Width, 0.1f);
            ImGui::DragFloat("Height", &m_PatchRectCreationParameters.Height, 0.1f);

            if (ImGui::Button("Cancel"))
            {
                m_ShowBSplinePatchRectCreationPopup = false;
            }
            ImGui::SameLine();
            if (ImGui::Button("Create"))
            {
                m_Scene->CreateBSplinePatchRect("BSplinePatchRect", m_PatchRectCreationParameters);
                m_ShowBSplinePatchRectCreationPopup = false;
            }

            ImGui::EndPopup();

        }

        if (m_ShowBSplinePatchCylinderCreationPopup)
        {
            ImGui::OpenPopup("BSplinePatch Cylinder Creation");
        }

        if (ImGui::BeginPopupModal("BSplinePatch Cylinder Creation", &m_ShowBSplinePatchCylinderCreationPopup))
        {
            int patchX = m_PatchCylinderCreationParameters.PatchCountX;
            if (ImGui::DragInt("PatchCountX", &patchX, 1.0f, 3))
            {
                patchX = std::max(patchX, 3);
                m_PatchCylinderCreationParameters.PatchCountX = patchX;
            }
            ImGui::DragInt("PatchCountY", &m_PatchCylinderCreationParameters.PatchCountY, 1.0f, 1);
            ImGui::DragFloat("Radius", &m_PatchCylinderCreationParameters.Radius, 0.1f);
            ImGui::DragFloat("Height", &m_PatchCylinderCreationParameters.Height, 0.1f);

            if (ImGui::Button("Cancel"))
            {
                m_ShowBSplinePatchCylinderCreationPopup = false;
            }
            ImGui::SameLine();
            if (ImGui::Button("Create"))
            {
                m_Scene->CreateBSplinePatchCylinder("BSplinePatch Cylinder", m_PatchCylinderCreationParameters);
                m_ShowBSplinePatchCylinderCreationPopup = false;
            }

            ImGui::EndPopup();
        }
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

        ImGui::Checkbox("ShowPoints", &Renderer::ShowPoints);
        ImGui::Checkbox("ShowGrid", &m_ShowGrid);
        if (ImGui::DragInt("PointSize", &Renderer::PointSize, 1.0f, 1.0f, 10.0f))
        {
            Renderer::PointSize = std::clamp(Renderer::PointSize, 1, 10);
            glPointSize(Renderer::PointSize);
        }

        ImGui::EndGroup();

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

        ImGui::BeginGroup();
        ImGui::Text("Stereo");
        if (ImGui::Checkbox("Enable stereoscopic", &m_EnableStereoscopic))
        {
            if (m_EnableStereoscopic)
                glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            else
            {
                glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
                m_Scene->SetDefaultColor(Renderer::DEFAULT_COLOR);
                m_Scene->SetSelectionColor(Renderer::SELECTED_COLOR);
            }
        }

        auto eyeDistance = m_CameraController.GetCamera().GetEyeDistance();
        if (ImGui::DragFloat("Eye Distance", &eyeDistance, 0.001f))
        {
            m_CameraController.GetCamera().SetEyeDistance(eyeDistance);
        }

        auto projectionPlaneDistance = m_CameraController.GetCamera().GetProjectionPlaneDistance();
        if (ImGui::DragFloat("ProjectionPlaneDistance", &projectionPlaneDistance, 0.1f))
        {
            m_CameraController.GetCamera().SetProjectionPlaneDistance(projectionPlaneDistance);
        }

        //ImGui::ColorEdit3("LeftFilter", &m_LeftFilter.x);
        //ImGui::ColorEdit3("RightFilter", &m_RightFilter.x);

        ImGui::EndGroup();

        ImGui::End();
    }

    void EditorLayer::RenderCursor(glm::vec3 position, float cursorSize, const glm::vec4& color)
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

        if (m_EnableStereoscopic)
        {
            Renderer::RenderLine(startX, endX, color);
            Renderer::RenderLine(startY, endY, color);
            Renderer::RenderLine(startZ, endZ, color);
        }
        else
        {
            Renderer::RenderLine(startX, endX, redColor);
            Renderer::RenderLine(startY, endY, greenColor);
            Renderer::RenderLine(startZ, endZ, blueColor);
        }
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
