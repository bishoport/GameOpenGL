#include "Scene.h"
#include "../Core/EngineOpenGL.h"

entt::entity Scene::EntityCreator(Ref<libCore::ModelContainer> modelContainer)
{
	entt::entity entity = EntityManager::GetInstance().m_registry.create();
	EntityManager::GetInstance().m_registry.emplace<libCore::Renderer>(entity, modelContainer);
	modelsInScene.push_back(modelContainer);
	entitiesDictionary.emplace(entitiesDictionary.size() + 1, entity);
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

	EntityCreator(libCore::EngineOpenGL::GetInstance().CreateRoof());
	EntityCreator(libCore::EngineOpenGL::GetInstance().CreateRoof());
	EntityCreator(libCore::EngineOpenGL::GetInstance().CreateRoof());

}

void Scene::Tick(float DeltaTime)
{
}


