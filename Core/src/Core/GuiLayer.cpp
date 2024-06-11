#include "GuiLayer.h"
#include "../tools/ShaderManager.h"

namespace libCore
{
    GuiLayer::GuiLayer(GLFWwindow* window, float window_W, float window_H)
    {
        m_window_W = window_W;
        m_window_H = window_H;

        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        // Enable Docking
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
        //io.ConfigFlags  |= ImGuiConfigFlags_ViewportsEnable;      // Enable Multi-Viewport / Platform Windows
        io.Fonts->Flags |= ImFontAtlasFlags_NoBakedLines;
        io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleViewports;
        io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleFonts;

        io.ConfigWindowsMoveFromTitleBarOnly = true;
        io.ConfigDragClickToInputText = true;
        io.ConfigDockingTransparentPayload = true;

        // Setup Platform/Renderer bindings
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 460");

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();

        // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
        ImGuiStyle& style = ImGui::GetStyle();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }


        //--ImGUI Configuration--------------------
        //std::ifstream ifile("imgui.ini");
        //ini_file_exists = ifile.is_open();
        //ifile.close();
        //
        //if (ini_file_exists)
        //    ImGui::LoadIniSettingsFromDisk("imgui.ini");

        //FONTS
        //io.Fonts->AddFontFromFileTTF("assets/default/Fonts/sourceSans/SourceSansPro-Regular.otf", 18);
        //io.Fonts->AddFontFromFileTTF("assets/default/Fonts/source-code-pro/SourceCodePro-Regular.otf", 26);

        // Despu�s de cargar las fuentes, llama a esta funci�n
        //ImGui_ImplOpenGL3_CreateFontsTexture();
    }

    GuiLayer::~GuiLayer() {
        // Cleanup
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void GuiLayer::begin() {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void GuiLayer::renderDockers()
    {
        // Configuración del Docking
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;//  ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f)); // Make dockspace background transparent

        ImGui::Begin("DockSpace", nullptr, window_flags);
        ImGui::PopStyleVar(3);

        // DockSpace
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);

        //ImGui::End();
        ImGui::PopStyleColor(); // Revert the transparent background color
    }

    void GuiLayer::renderMainMenuBar() {

        // Barra de menu principal
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Open", "Ctrl+O")) { /* Abrir archivo */ }
                if (ImGui::MenuItem("Save", "Ctrl+S")) { /* Guardar archivo */ }
                //if (ImGui::MenuItem("Exit", "Ctrl+Q")) { glfwSetWindowShouldClose(window, true); }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Edit"))
            {
                if (ImGui::MenuItem("Undo", "Ctrl+Z")) { /* Deshacer */ }
                if (ImGui::MenuItem("Redo", "Ctrl+Y")) { /* Rehacer */ }
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }
    }


    void GuiLayer::DrawViewports(std::vector<Ref<Viewport>> viewports)
    {
        for (auto& viewport : viewports)
        {
            std::string windowTitle = viewport->viewportName;
            ImGui::Begin(windowTitle.c_str(), nullptr);

            // Obtener los tamaños y posiciones actuales de la ventana
            ImVec2 currentViewportSize = ImGui::GetWindowSize();
            ImVec2 currentViewportPos = ImGui::GetWindowPos();

            // Verificar si se está arrastrando la ventana
            bool isCurrentlyDragging = ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows) && ImGui::IsMouseDragging(ImGuiMouseButton_Left);

            // Verificar si el tamaño ha cambiado
            bool isCurrentlyResizing = currentViewportSize.x != viewport->previousViewportSize.x || currentViewportSize.y != viewport->previousViewportSize.y;

            // Detectar cuándo se comienza a mover o redimensionar
            if (!viewport->isMoving && isCurrentlyDragging)
            {
                viewport->isMoving = true;
            }
            if (!viewport->isResizing && isCurrentlyResizing)
            {
                viewport->isResizing = true;
            }

            // Detectar cuándo termina el movimiento o el redimensionamiento
            if (viewport->isMoving && !isCurrentlyDragging)
            {
                viewport->isMoving = false;
                EventManager::OnPanelResizedEvent().trigger(viewport->viewportName, glm::vec2(currentViewportSize.x, currentViewportSize.y),
                    glm::vec2(currentViewportPos.x, currentViewportPos.y));
            }
            if (viewport->isResizing && !isCurrentlyResizing)
            {
                viewport->isResizing = false;
                EventManager::OnPanelResizedEvent().trigger(viewport->viewportName, glm::vec2(currentViewportSize.x, currentViewportSize.y),
                    glm::vec2(currentViewportPos.x, currentViewportPos.y));
            }

            // Actualiza los valores anteriores
            viewport->previousViewportSize = currentViewportSize;
            viewport->previousViewportPos = currentViewportPos;

            // Actualiza los valores actuales en el viewport
            viewport->viewportSize = currentViewportSize;
            viewport->viewportPos = currentViewportPos;

            // Renderiza la imagen
            ImGui::Image((void*)(intptr_t)viewport->framebuffer_final->getTexture("color"), ImVec2(viewport->viewportSize.x, viewport->viewportSize.y), ImVec2(0, 1), ImVec2(1, 0), ImColor(255, 255, 255, 255));
            viewport->mouseInviewport = false;
            ImGui::End();
        }
    }

    void GuiLayer::DrawHierarchyPanel(const std::vector<Ref<libCore::ModelContainer>>& modelsInScene)
    {
        ImGui::Begin("Hierarchy");
        //for (auto& modelContainer : modelsInScene) {

        //    if (ImGui::TreeNode(modelContainer.get(), "Model: %s", modelContainer->name.c_str())) {
        //        for (int j = 0; j < modelContainer->models.size(); j++) {
        //            if (ImGui::TreeNode(modelContainer->models[j].get(), "Child: %d", j)) {
        //                // Aqu� listamos las Meshes de cada Model
        //                auto& model = modelContainer->models[j];
        //                for (int k = 0; k < model->meshes.size(); k++) {
        //                    ImGui::BulletText("Mesh: %s", model->meshes[k]->meshName.c_str());
        //                    ImGui::BulletText("Transform:");
        //                    ImGui::DragFloat3("Position", &model->transform.position[0], 0.1f);
        //                    ImGui::DragFloat3("Rotation", &model->transform.rotation[0], 0.01f, -3.14159f, 3.14159f, "%.3f rad");
        //                    ImGui::DragFloat3("Scale", &model->transform.scale[0], 0.1f, 0.01f, 100.0f, "%.3f");
        //                }
        //                ImGui::TreePop(); // Finaliza el nodo del Model
        //            }
        //        }
        //        ImGui::TreePop(); // Finaliza el nodo del ModelContainer
        //    }
        //}

       for (auto& modelContainer : modelsInScene) {

           entt::entity newEntity = Scene::GetInstance().entitiesDictionary[modelContainer->entityIdentifier];
           std::string nameEntity = EntityManager::GetInstance().m_registry.get<libCore::ConfigComp>(newEntity).name;

            if (ImGui::TreeNode(&newEntity, nameEntity.c_str())) {
                if (EntityManager::GetInstance().m_registry.has<Transform>(newEntity)) {
                    for (int j = 0; j < modelContainer->models.size(); j++) {
                        if (ImGui::TreeNode(modelContainer->models[j].get(), "Child: %d", j)) {
                            // Aqu� listamos las Meshes de cada Model
                            auto& model = modelContainer->models[j];
                            for (int k = 0; k < model->meshes.size(); k++) {
                                ImGui::BulletText("Mesh: %s", model->meshes[k]->meshName.c_str());
                                ImGui::BulletText("Transform:");
                                ImGui::DragFloat3("Position", &model->transform.position[0], 0.1f);
                                ImGui::DragFloat3("Rotation", &model->transform.rotation[0], 0.01f, -3.14159f, 3.14159f, "%.3f rad");
                                ImGui::DragFloat3("Scale", &model->transform.scale[0], 0.1f, 0.01f, 100.0f, "%.3f");
                            }
                            ImGui::TreePop(); // Finaliza el nodo del Model
                        }
                    }
                }

                if (EntityManager::GetInstance().m_registry.has<libCore::Renderer>(newEntity)) {
                    if (ImGui::TreeNode(&EntityManager::GetInstance().m_registry.get<libCore::Renderer>(newEntity), "Renderer"))
                    {
                        ImGui::BulletText("TESTING");
                        ImGui::Checkbox("Render Bounding Box", &EntityManager::GetInstance().m_registry.get<libCore::Renderer>(newEntity).modelContainerData->isBoundingBox);
                        ImGui::TreePop();
                    }
                }
                ImGui::TreePop(); // Finaliza el nodo del ModelContainer

            }
        }
        ImGui::End();
    }

    void GuiLayer::DrawLightsPanel(const std::vector<Ref<libCore::Light>>& lightsInScene)
    {
        ImGui::Begin("Lights");

        for (size_t i = 0; i < lightsInScene.size(); ++i) {
            auto& light = lightsInScene[i];

            // Push a unique identifier onto the ID stack
            ImGui::PushID(static_cast<int>(i));

            ImGui::BulletText("Light: %d", light->id);
            ImGui::BulletText("Transform:");
            ImGui::DragFloat3("Position", &light->transform.position[0], 0.1f);
            ImGui::DragFloat3("Rotation", &light->transform.rotation[0], 0.01f, -3.14159f, 3.14159f, "%.3f rad");
            ImGui::DragFloat3("Scale", &light->transform.scale[0], 0.1f, 0.01f, 100.0f, "%.3f");

            // Attenuation parameters
            ImGui::DragFloat("Linear", &light->Linear, 0.01f, 0.0f, 10.0f, "%.2f");
            ImGui::DragFloat("Quadratic", &light->Quadratic, 0.01f, 0.0f, 10.0f, "%.2f");
            ImGui::DragFloat("Radius", &light->Radius, 0.01f, 0.0f, 100.0f, "%.2f");

            // Color Picker
            ImGui::ColorEdit3("Color", &light->color[0]);

            // Pop the identifier off the ID stack
            ImGui::PopID();
        }

        ImGui::End();
    }




    void GuiLayer::end()
    {
        ImGui::End(); // End of Docking Example window

        ImGuiIO& io = ImGui::GetIO();
        io.DisplaySize = ImVec2(m_window_W, m_window_H);

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
