#pragma once
#include "../LibCoreHeaders.h"
#include "../Timestep.h"
#include "../tools/AssetsManager.h"
#include "../tools/ModelLoader.h"
#include "../tools/PrimitivesHelper.h"
#include "Light.hpp"
#include "../ECS/Scene.h"

namespace libCore
{
    //class GuiLayer;
    class EngineOpenGL
    {
        using MainLoopFnc = std::function<void(Timestep)>;
        using CloseGlFnc = std::function<void()>;
        using KeyPressedFnc = std::function<void(int)>;
        using ImGUILoopFnc = std::function<void()>;

    public:
        bool InitializeEngine(const std::string& windowTitle, int initialWindowWidth, int initialWindowHeight, MainLoopFnc mainLoopFnc, CloseGlFnc closeGlFnc, ImGUILoopFnc imGUILoopFnc = [] {});
        void InitializeMainLoop();
        void begin();
        void StopMainLoop();
        void SetupInputCallbacks();


        //VIEWPORTS
        void CreateViewport(std::string name, glm::vec3 cameraPosition);
        void RenderViewports(const std::vector<Ref<libCore::ModelContainer>>& modelsInScene);
        void DrawViewports_ImGUI();


        //PANELS
        void DrawHierarchyPanel(const std::vector<Ref<libCore::ModelContainer>>& modelsInScene);


        //GENERACION DE PRIMITIVAS
        Ref<ModelContainer> CreatePrefabDot(const glm::vec3& pos);
        Ref<ModelContainer> CreatePrefabLine(const glm::vec3& point1, const glm::vec3& point2);
        Ref<ModelContainer> CreatePrefabCube();
        Ref<ModelContainer> CreatePrefabSphere(float radius, unsigned int sectorCount, unsigned int stackCount);
        Ref<ModelContainer> CreateRoof();
        void AddBounding(Ref<libCore::ModelContainer>& modelLoader);
        void AttachBounding(Ref<libCore::Model>& attachToModel);

        // Función estática para obtener la instancia de EngineOpenGL
        static EngineOpenGL& GetInstance()
        {
            static EngineOpenGL instance;
            return instance;
        }
        // Función estática para obtener la ventana GLFW
        static GLFWwindow* GetWindow()
        {
            return GetInstance().window;
        }
 
    private:

        GLFWwindow* window;
        Timestep m_deltaTime;

        MainLoopFnc g_mainLoodFnc;
        CloseGlFnc g_closeGlFnc;
        KeyPressedFnc g_keyPressedFnc;
        ImGUILoopFnc g_imGUILoopFnc;

        libCore::ShaderManager shaderManager;

        bool running = false;
        bool useImGUI = true;
        bool renderInImGUI = false;

        int windowWidth = 0;
        int windowHeight = 0;


        // Constructor privado para el patrón Singleton
        EngineOpenGL() = default;

        // Deshabilitar copia y asignación
        EngineOpenGL(const EngineOpenGL&) = delete;
        EngineOpenGL& operator=(const EngineOpenGL&) = delete;






        //--------------------------ESTO VA FUERA-----------------------------------------------------------
        // lighting info
        // -------------

        float ambientLight = 1.0f;

        bool hdrEnabled = false;
        float hdrExposure = 1.0f;

        const unsigned int NR_LIGHTS = 0;
        std::vector<Ref<Light>> lights;


        GLuint quadVAO, quadVBO;
        void setupQuad() {
            float quadVertices[] = {
                // positions  // texCoords
                -1.0f,  1.0f, 0.0f, 1.0f,
                -1.0f, -1.0f, 0.0f, 0.0f,
                 1.0f, -1.0f, 1.0f, 0.0f,

                -1.0f,  1.0f, 0.0f, 1.0f,
                 1.0f, -1.0f, 1.0f, 0.0f,
                 1.0f,  1.0f, 1.0f, 1.0f
            };

            glGenVertexArrays(1, &quadVAO);
            glGenBuffers(1, &quadVBO);
            glBindVertexArray(quadVAO);
            glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
            glBindVertexArray(0);
        }

        void renderQuad()
        {
            glBindVertexArray(quadVAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glBindVertexArray(0);
        }

    };
}
