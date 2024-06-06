#include "MapEditor.h"
#include <Core/EngineOpenGL.h>


void MapEditor::Init()
{
    bool ok = libCore::EngineOpenGL::GetInstance().InitializeEngine("MAP EDITOR", screenWidth, screenHeight,
        std::bind(&MapEditor::LoopOpenGL, this, std::placeholders::_1),
        std::bind(&MapEditor::OnCloseOpenGL, this),
        std::bind(&MapEditor::LoopImGUI, this));
    if (!ok) return;
    //-----------------------------------------------------------------


    // -- OPENGL FLAGS
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    //-----------------------------


    // -- PREPARE GEOMETRY
    //libCore::ImportModelData importModelData;
    //importModelData.filePath = "assets/models/Robot/";
    //importModelData.fileName = "Robot.fbx";
    //importModelData.invertUV = false;
    //importModelData.rotate90 = false;
    //importModelData.useCustomTransform = true;
    //importModelData.modelID = 1;
    //importModelData.globalScaleFactor = 1.0f;

    //libCore::ImportModelData importModelData2;
    //importModelData2.filePath = "assets/models/Turbina/";
    //importModelData2.fileName = "turbina.fbx";
    //importModelData2.invertUV = false;
    //importModelData2.rotate90 = false;
    //importModelData2.useCustomTransform = true;
    //importModelData2.modelID = 1;
    //importModelData2.globalScaleFactor = 0.1f;

    //libCore::ImportModelData importModelData3;
    //importModelData3.filePath = "assets/models/cp/cyberpunk/";
    //importModelData3.fileName = "c11retro.fbx";
    //importModelData3.invertUV = false;
    //importModelData3.rotate90 = false;
    //importModelData3.useCustomTransform = true;
    //importModelData3.modelID = 1;
    //importModelData3.globalScaleFactor = 1.0f;

    //modelsInScene.push_back(libCore::ModelLoader::LoadModel(importModelData));
    //modelsInScene.push_back(libCore::ModelLoader::LoadModel(importModelData2));
    //modelsInScene.push_back(libCore::ModelLoader::LoadModel(importModelData3));


    //Ejemplo de figura con puntos y lineas por separado
    //modelsInScene.push_back(libCore::EngineOpenGL::GetInstance().CreatePrefabDot(glm::vec3(-2.0f, 0.0f,  2.0f)));
    //modelsInScene.push_back(libCore::EngineOpenGL::GetInstance().CreatePrefabDot(glm::vec3(-2.0f, 0.0f, -2.0f)));
    //modelsInScene.push_back(libCore::EngineOpenGL::GetInstance().CreatePrefabDot(glm::vec3( 2.0f, 0.0f, -2.0f)));
    //modelsInScene.push_back(libCore::EngineOpenGL::GetInstance().CreatePrefabDot(glm::vec3( 2.0f, 0.0f,  2.0f)));
    //modelsInScene.push_back(libCore::EngineOpenGL::GetInstance().CreatePrefabLine(glm::vec3(-2.0f, 0.0f, 2.0f),glm::vec3(-2.0f, 0.0f, -2.0f)));
    //modelsInScene.push_back(libCore::EngineOpenGL::GetInstance().CreatePrefabLine(glm::vec3(-2.0f, 0.0f, -2.0f),glm::vec3(2.0f, 0.0f, -2.0f)));
    //modelsInScene.push_back(libCore::EngineOpenGL::GetInstance().CreatePrefabLine(glm::vec3(2.0f, 0.0f, -2.0f),glm::vec3(2.0f, 0.0f, 2.0f)));
    //modelsInScene.push_back(libCore::EngineOpenGL::GetInstance().CreatePrefabLine(glm::vec3(2.0f, 0.0f, 2.0f),glm::vec3(-2.0f, 0.0f, 2.0f)));


    //modelsInScene.push_back(libCore::EngineOpenGL::GetInstance().CreatePrefabCube());


  //  modelsInScene.push_back(libCore::EngineOpenGL::GetInstance().CreateRoof());






    // -- VIEWPORTS
    libCore::EngineOpenGL::GetInstance().CreateViewport("VIEWPORT_1", glm::vec3(0.0f, 0.0f, 5.0f));
    //libCore::EngineOpenGL::GetInstance().CreateViewport("VIEWPORT_2", glm::vec3(0.0f, 1.0f, 7.0f));
    //------------------------------------------------------------------


    // -- START LOOP OpenGL
    libCore::EngineOpenGL::GetInstance().InitializeMainLoop();
    //------------------------------------------------------------------
}


void MapEditor::LoopOpenGL(libCore::Timestep deltaTime)
{
    libCore::EngineOpenGL::GetInstance().RenderViewports(Scene::GetInstance().modelsInScene); 
}

void MapEditor::LoopImGUI()
{
    libCore::EngineOpenGL::GetInstance().DrawHierarchyPanel(Scene::GetInstance().modelsInScene);
}

void MapEditor::OnCloseOpenGL()
{
}
