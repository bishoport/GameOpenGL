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

    //Aqui llega la funcion para que el editor de Roofs devuelva a quien sea, la matriz de footPrints
    void libCore::GuiLayer::SetCallBackFunc(CallbackFromGuiLayer callbackFromGuiLayerFunc)
    {
        m_callbackFromGuiLayerFunc = callbackFromGuiLayerFunc;
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

    /*    gizmoMove = Ref<IGizmo>(CreateMoveGizmo());
        gizmoRotate = Ref<IGizmo>(CreateRotateGizmo());
        gizmoScale = Ref<IGizmo>(CreateScaleGizmo());

        gizmo = gizmoMove;*/
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
            std::string nameEntity = EntityManager::GetInstance().m_registry.get<libCore::EntityInfo>(newEntity).name;

            if (ImGui::TreeNode(&newEntity, nameEntity.c_str())) {

                if (EntityManager::GetInstance().m_registry.has<libCore::EntityInfo>(newEntity)) {
                    if (ImGui::TreeNode(&EntityManager::GetInstance().m_registry.get<libCore::EntityInfo>(newEntity), "Entity")) {
                        ImGui::BulletText("Entity Customization");
                        ImGui::InputText("Name:", &EntityManager::GetInstance().m_registry.get<libCore::EntityInfo>(newEntity).name);
                       
                        static int tagIdx = EntityManager::GetInstance().m_registry.get<libCore::EntityInfo>(newEntity).indexTag;
                        std::vector<const char*> arrayTag;
                        for (unsigned i = 0; i < Scene::GetInstance().tags.size(); i++) {
                            arrayTag.push_back(Scene::GetInstance().tags[i].c_str());
                        }
                        ImGui::Combo("Tag System", &tagIdx, arrayTag.data(), arrayTag.size());

                        EntityManager::GetInstance().m_registry.get<libCore::EntityInfo>(newEntity).indexTag = tagIdx;
                        static int layerIdx = EntityManager::GetInstance().m_registry.get<libCore::EntityInfo>(newEntity).indexLayer;
                        std::vector<const char*> arrayLayers;
                        for (unsigned i = 0; i < Scene::GetInstance().layers.size(); i++) {
                            arrayLayers.push_back(Scene::GetInstance().layers[i].c_str());
                        }
                        ImGui::Combo("Layer System", &layerIdx, arrayLayers.data(), arrayLayers.size());
                        EntityManager::GetInstance().m_registry.get<libCore::EntityInfo>(newEntity).indexLayer = layerIdx;

                        ImGui::TreePop();
                    }
                }

                if (EntityManager::GetInstance().m_registry.has<Transform>(newEntity)) {
                    for (int j = 0; j < modelContainer->models.size(); j++) {
                        if (ImGui::TreeNode(modelContainer->models[j].get(), "Child: %d", j)) {
                            // Aqu� listamos las Meshes de cada Model
                            auto& model = modelContainer->models[j];
                            ImGui::BulletText("Transform:");
                            ImGui::DragFloat3("Position", &model->transform.position[0], 0.1f);
                            ImGui::DragFloat3("Rotation", &model->transform.rotation[0], 0.01f, -3.14159f, 3.14159f, "%.3f rad");
                            ImGui::DragFloat3("Scale", &model->transform.scale[0], 0.1f, 0.01f, 100.0f, "%.3f");
                            ImGui::TreePop(); // Finaliza el nodo del Model
                        }
                    }
                }

                if (EntityManager::GetInstance().m_registry.has<libCore::Renderer>(newEntity)) {
                    if (ImGui::TreeNode(&EntityManager::GetInstance().m_registry.get<libCore::Renderer>(newEntity), "Renderer"))
                    {
                        ImGui::BulletText("Renderer");
                        ImGui::Checkbox("Render Bounding Box", &EntityManager::GetInstance().m_registry.get<libCore::Renderer>(newEntity).modelContainerData->isBoundingBox);
                        ImGui::Checkbox("Don't Draw", &EntityManager::GetInstance().m_registry.get<libCore::Renderer>(newEntity).modelContainerData->isDontDraw);

                        ImGui::TreePop();
                    }
                }
                ImGui::TreePop(); // Finaliza el nodo del ModelContainer

            }
        }
        ImGui::End();
    }

    void GuiLayer::GizmosBasicButtons(float width, float height)
    {
        ImGui::SetNextWindowPos(ImVec2(width / 20, height / 20), ImGuiCond_Always);
        ImGui::SetNextWindowBgAlpha(0.0f);
        ImGui::Begin("Main Window", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoResize);
        if (ImGui::Button("Translation")) {
            currentOperation = ImGuizmo::OPERATION::TRANSLATE;
        }
        ImGui::SameLine();

        if (ImGui::Button("Rotation")) {
            currentOperation = ImGuizmo::OPERATION::ROTATE;
        }
        ImGui::SameLine();

        if (ImGui::Button("Scale")) {
            currentOperation = ImGuizmo::OPERATION::SCALE;
        }

        ImGui::DragFloat("Snap", &snappingGizmo, 0.01f);
        ImGui::End();
    }

    void GuiLayer::SelectCurrentGizmoObject(const std::vector<Ref<libCore::ModelContainer>>& modelsInScene, libCore::Camera camera)
    {
        for (auto& modelContainer : modelsInScene) {

            entt::entity newEntity = Scene::GetInstance().entitiesDictionary[modelContainer->entityIdentifier];
            std::string nameEntity = EntityManager::GetInstance().m_registry.get<libCore::EntityInfo>(newEntity).name;
            if (EntityManager::GetInstance().m_registry.has<Transform>(newEntity)) {
                if (nameEntity == "Guts") {
                    DrawGizmos(newEntity, camera, modelContainer);
                }
            }
           
        }
    }

    void GuiLayer::DrawGizmos(entt::entity& entity, libCore::Camera camera, Ref<libCore::ModelContainer> modelContainer)
    {
        ImGuizmo::SetOrthographic(false);
        ImGuizmo::SetDrawlist();

        float windowWidth = ImGui::GetIO().DisplaySize.x;
        float windowHeight = ImGui::GetIO().DisplaySize.y;
        ImGuizmo::SetRect(0, 0, windowWidth, windowHeight);
        glm::mat4 transformComp = EntityManager::GetInstance().m_registry.get<libCore::Transform>(entity).getMatrix();
        float snap[3] = { snappingGizmo, snappingGizmo, snappingGizmo };
        ImGuizmo::Manipulate(glm::value_ptr(camera.view), glm::value_ptr(camera.projection),
         currentOperation, ImGuizmo::WORLD, glm::value_ptr(transformComp), NULL, snap);


        if (transformComp != EntityManager::GetInstance().m_registry.get<libCore::Transform>(entity).getMatrix()) {
            EntityManager::GetInstance().m_registry.get<libCore::Transform>(entity).setMatrix(transformComp);

            for (unsigned i = 0; i < modelContainer->models.size(); i++) {
                modelContainer->models[i]->transform.position = EntityManager::GetInstance().m_registry.get<libCore::Transform>(entity).position;
                modelContainer->models[i]->transform.rotation = EntityManager::GetInstance().m_registry.get<libCore::Transform>(entity).rotation;
                modelContainer->models[i]->transform.scale = EntityManager::GetInstance().m_registry.get<libCore::Transform>(entity).scale;
            }

        }
     

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


    //Especial para el editor de Roofs
    void GuiLayer::RenderCheckerMatrix() {
        const int xAxis = 10;
        const int yAxis = 10;
        const float scale = 1.0;

        static bool checkerMatrix[xAxis][yAxis] = { false }; // Matriz de checkboxes de 10x10
        static bool holeMatrix[xAxis][yAxis] = { false };    // Matriz de checkboxes para los agujeros
        static std::vector<Point> selectedPoints;     // Vector para almacenar los puntos seleccionados
        static std::vector<Point> holePoints;         // Vector para almacenar los puntos de los agujeros
        static std::vector<std::pair<int, int>> selectedOrder; // Para almacenar el orden de selección de celdas
        static std::vector<std::pair<int, int>> holeOrder;     // Para almacenar el orden de selección de celdas para los agujeros

        ImGui::Begin("Checker Matrix");

        // Desplegable para la matriz de foot prints
        if (ImGui::CollapsingHeader("Foot Prints Matrix", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Text("Foot Prints:");

            // Generar la matriz de checkboxes
            for (int row = 0; row < xAxis; ++row) {
                for (int col = 0; col < yAxis; ++col) {
                    ImGui::PushID(row * yAxis + col); // Asegurar identificadores únicos

                    bool oldValue = checkerMatrix[row][col];
                    if (ImGui::Checkbox("", &checkerMatrix[row][col])) {
                        if (checkerMatrix[row][col]) {
                            // Añadir celda a la lista de selección si se marca
                            selectedOrder.emplace_back(row, col);
                        }
                        else {
                            // Eliminar celda de la lista de selección si se desmarca
                            auto it = std::remove(selectedOrder.begin(), selectedOrder.end(), std::make_pair(row, col));
                            selectedOrder.erase(it, selectedOrder.end());
                        }
                    }
                    ImGui::PopID();

                    if (col < yAxis - 1) ImGui::SameLine(); // Mantener los checkboxes en la misma línea
                }
            }
        }

        // Desplegable para la matriz de holes
        if (ImGui::CollapsingHeader("Holes Matrix", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Text("Holes:");

            // Generar la matriz de checkboxes para los agujeros
            for (int row = 0; row < xAxis; ++row) {
                for (int col = 0; col < yAxis; ++col) {
                    ImGui::PushID((row * yAxis + col) + (xAxis * yAxis)); // Asegurar identificadores únicos

                    bool oldValue = holeMatrix[row][col];
                    if (ImGui::Checkbox("", &holeMatrix[row][col])) {
                        if (holeMatrix[row][col]) {
                            // Añadir celda a la lista de selección si se marca
                            holeOrder.emplace_back(row, col);
                        }
                        else {
                            // Eliminar celda de la lista de selección si se desmarca
                            auto it = std::remove(holeOrder.begin(), holeOrder.end(), std::make_pair(row, col));
                            holeOrder.erase(it, holeOrder.end());
                        }
                    }
                    ImGui::PopID();

                    if (col < yAxis - 1) ImGui::SameLine(); // Mantener los checkboxes en la misma línea
                }
            }
        }

        // Botón para recoger los resultados
        if (ImGui::Button("Generate Roof")) {
            selectedPoints.clear(); // Limpiar el vector de puntos seleccionados
            for (const auto& cell : selectedOrder) {
                int row = cell.first;
                int col = cell.second;
                if (checkerMatrix[row][col]) {
                    // Convertir la posición de la matriz en coordenadas (x, y)
                    float x = static_cast<float>(col) * scale; // Dividir por scale para escalar
                    float y = static_cast<float>(9 - row) * scale; // Invertir Y para que (0,0) esté en la parte inferior izquierda
                    selectedPoints.push_back(Point{ x, y });
                }
            }

            holePoints.clear(); // Limpiar el vector de puntos de los agujeros
            for (const auto& cell : holeOrder) {
                int row = cell.first;
                int col = cell.second;
                if (holeMatrix[row][col]) {
                    // Convertir la posición de la matriz en coordenadas (x, y)
                    float x = static_cast<float>(col) * scale; // Dividir por scale para escalar
                    float y = static_cast<float>(9 - row) * scale; // Invertir Y para que (0,0) esté en la parte inferior izquierda
                    holePoints.push_back(Point{ x, y });
                }
            }

            // Llamar al callback con los puntos seleccionados
            if (m_callbackFromGuiLayerFunc) {
                std::vector<Vector2d> vector2dPoints;
                std::vector<Vector2d> vector2dHolePoints;
                for (const auto& point : selectedPoints) {
                    vector2dPoints.push_back(Vector2d(point.x, point.y));
                }
                for (const auto& point : holePoints) {
                    vector2dHolePoints.push_back(Vector2d(point.x, point.y));
                }
                m_callbackFromGuiLayerFunc(vector2dPoints, vector2dHolePoints);
            }
        }

        // Botón para limpiar la matriz y la lista de puntos seleccionados
        if (ImGui::Button("Clear")) {
            selectedPoints.clear();
            holePoints.clear();
            selectedOrder.clear();
            holeOrder.clear();
            std::fill(&checkerMatrix[0][0], &checkerMatrix[0][0] + sizeof(checkerMatrix), false);
            std::fill(&holeMatrix[0][0], &holeMatrix[0][0] + sizeof(holeMatrix), false);
        }

        // Mostrar los puntos seleccionados en el formato requerido con índice
        ImGui::Text("Selected Points:");
        for (size_t i = 0; i < selectedPoints.size(); ++i) {
            const auto& point = selectedPoints[i];
            ImGui::Text("%zu: { %.1f, %.1f },", i, point.x, point.y);
        }

        // Mostrar los puntos de los agujeros en el formato requerido con índice
        ImGui::Text("Hole Points:");
        for (size_t i = 0; i < holePoints.size(); ++i) {
            const auto& point = holePoints[i];
            ImGui::Text("%zu: { %.1f, %.1f },", i, point.x, point.y);
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
