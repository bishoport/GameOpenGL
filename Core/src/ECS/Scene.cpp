#include "Scene.h"
#include "../Core/EngineOpenGL.h"

void Scene::BeginPlay()
{
	//Ref<libCore::ImportModelData> importModelData;
	//entityRobot = EntityManager::GetInstance().m_registry.create();
	//EntityManager::GetInstance().m_registry.emplace<libCore::Renderer>(entityRobot, importModelData, "Robot.fbx", "assets/models/Robot/");
	////modelsInScene.emplace(EntityManager::GetInstance().m_registry.get<libCore::Renderer>(entityRobot).importModelData);
	//entityOperator = EntityManager::GetInstance().m_registry.create();
	//EntityManager::GetInstance().m_registry.emplace<libCore::Renderer>(entityOperator, importModelData, "robot_operator.fbx", "assets/models/Robot/");
	//entitySecondOperator = EntityManager::GetInstance().m_registry.create();
	//EntityManager::GetInstance().m_registry.emplace<libCore::Renderer>(entitySecondOperator, importModelData, "robot_operator.fbx", "assets/models/Robot/");
	
	modelsInScene.push_back(libCore::EngineOpenGL::GetInstance().CreateRoof());
	modelsInScene.push_back(libCore::EngineOpenGL::GetInstance().CreateRoof());

	modelsInScene.push_back(libCore::EngineOpenGL::GetInstance().CreateRoof());

}

void Scene::Tick(float DeltaTime)
{
	libCore::EngineOpenGL::GetInstance().RenderViewports(modelsInScene);
}

void Scene::TickEditor(float DeltaTime)
{
	libCore::EngineOpenGL::GetInstance().DrawHierarchyPanel(modelsInScene);

}
