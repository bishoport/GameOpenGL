#pragma once
#include "Scene.h"
#include "../Core/EngineOpenGL.h"

entt::entity Scene::EntityCreator(Ref<libCore::ModelContainer> modelContainer, std::string nameEntity)
{

	unsigned index = entitiesDictionary.size() + 1;
	entt::entity entity = EntityManager::GetInstance().m_registry.create();
	modelContainer->entityIdentifier = index;
	EntityManager::GetInstance().m_registry.emplace<libCore::Transform>(entity);
	EntityManager::GetInstance().m_registry.emplace<libCore::Renderer>(entity, modelContainer);
	if (nameEntity.size() <= 0)
		nameEntity = "ACTOR_" + std::to_string(index);
	
	EntityManager::GetInstance().m_registry.emplace<libCore::ConfigComp>(entity, nameEntity);

	modelsInScene.push_back(modelContainer);
	entitiesDictionary.emplace(index, entity);
	return entity;
}

void Scene::BeginPlay()
{
	//Ref<libCore::ModelContainer> roof = libCore::EngineOpenGL::GetInstance().CreateRoof();

	/*modelsInScene.push_back(libCore::EngineOpenGL::GetInstance().CreateRoof());
	modelsInScene.push_back(libCore::EngineOpenGL::GetInstance().CreateRoof());

	modelsInScene.push_back(libCore::EngineOpenGL::GetInstance().CreateRoof());*/
	/*EntityCreator(libCore::EngineOpenGL::GetInstance().CreateRoof());
	EntityCreator(libCore::EngineOpenGL::GetInstance().CreateRoof());
	EntityCreator(libCore::EngineOpenGL::GetInstance().CreateRoof());*/

	EntityCreator(libCore::EngineOpenGL::GetInstance().CreateRoof(), "Roof 1");
	EntityCreator(libCore::EngineOpenGL::GetInstance().CreateRoof(), "Roof 2");
	EntityCreator(libCore::EngineOpenGL::GetInstance().CreateRoof(), "Roof 3");

}

void Scene::Tick(float DeltaTime)
{
}


