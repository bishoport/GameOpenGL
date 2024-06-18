#pragma once

#include "../LibCoreHeaders.h"
#include "../Timestep.h"
#include "../tools/AssetsManager.h"
#include "../tools/ModelLoader.h"
#include "../tools/PrimitivesHelper.h"
#include "Light.hpp"

#include "../StraightSkeleton/Vector2d.h"
#include "../tools/FreeTypeManager.h"
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


        //VIEWPORTS & RENDER
        void CreateViewport(std::string name, glm::vec3 cameraPosition);
        void RenderViewports();
        void DrawViewports_ImGUI();


        //GENERACION DE PRIMITIVAS
        Ref<libCore::ModelContainer>  CreatePrefabExternalModel(ImportModelData importModelData);
        Ref<libCore::ModelContainer>  CreatePrefabDot(const glm::vec3& pos, const glm::vec3& polygonColor);
        Ref<libCore::ModelContainer>  CreatePrefabLine(const glm::vec3& point1, const glm::vec3& point2);
        Ref<libCore::ModelContainer> CreatePrefabCube();
        Ref<libCore::ModelContainer>  CreatePrefabSphere(float radius, unsigned int sectorCount, unsigned int stackCount);
        Ref<libCore::ModelContainer>  CreateRoof(const std::vector<Vector2d>& points, const std::vector<Vector2d>& holes);

        //PANELS
        void DrawHierarchyPanel();


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
 
        void AddBounding(Ref<libCore::ModelContainer>& modelLoader);
        void AttachBounding(Ref<libCore::Model>& attachToModel);
   
    public:
        FreeTypeManager* freeTypeManager = nullptr;

    private:
        std::vector<glm::vec3> maxMinBounds(std::vector<glm::vec3> vertices, glm::mat4 model);
        glm::vec3 centerBounds(glm::vec3 max, glm::vec3 min);
        std::vector<glm::vec3> convertVertexToVector(std::vector<Vertex> vertexList);
        bool m_wireframe = false;

        std::vector<Ref<libCore::ModelContainer>> modelsInScene;

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

        const unsigned int NR_LIGHTS = 10;
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
