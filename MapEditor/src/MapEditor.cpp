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
    libCore::ImportModelData importModelData;
    importModelData.filePath = "assets/models/Robot/";
    importModelData.fileName = "Robot.fbx";
    importModelData.invertUV = false;
    importModelData.rotate90 = false;
    importModelData.useCustomTransform = true;
    importModelData.modelID = 1;
    importModelData.globalScaleFactor = 1.0f;
    libCore::EngineOpenGL::GetInstance().CreatePrefabExternalModel(importModelData);

    libCore::ImportModelData importModelData2;
    importModelData2.filePath = "assets/models/Turbina/";
    importModelData2.fileName = "turbina.fbx";
    importModelData2.invertUV = false;
    importModelData2.rotate90 = false;
    importModelData2.useCustomTransform = true;
    importModelData2.modelID = 1;
    importModelData2.globalScaleFactor = 0.1f;
    libCore::EngineOpenGL::GetInstance().CreatePrefabExternalModel(importModelData2);


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
    libCore::EngineOpenGL::GetInstance().RenderViewports(); 
}


void MapEditor::LoopImGUI()
{
    libCore::EngineOpenGL::GetInstance().DrawHierarchyPanel();
}

void MapEditor::OnCloseOpenGL()
{
}
