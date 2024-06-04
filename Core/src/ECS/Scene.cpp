#include "Scene.h"

void Scene::BeginPlay()
{
	Ref<libCore::ImportModelData> importModelData;
	entityRobot = EntityManager::GetInstance().m_registry.create();
	EntityManager::GetInstance().m_registry.emplace<libCore::Renderer>(entityRobot, importModelData, "Robot.fbx", "assets/models/Robot/");
	entityOperator = EntityManager::GetInstance().m_registry.create();
	EntityManager::GetInstance().m_registry.emplace<libCore::Renderer>(entityOperator, importModelData, "robot_operator.fbx", "assets/models/Robot/");
	entitySecondOperator = EntityManager::GetInstance().m_registry.create();
	EntityManager::GetInstance().m_registry.emplace<libCore::Renderer>(entitySecondOperator, importModelData, "robot_operator.fbx", "assets/models/Robot/");
}

void Scene::Tick(float DeltaTime)
{
}
