#pragma once
#include "Systems/RenderSystem.h"

#include "EntityManager.hpp"

class Scene : public Singleton<Scene> {
private:
	
	entt::entity EntityCreator(Ref<libCore::ModelContainer> modelContainer, std::string nameEntity = "");


public:


	RenderSystem systemRender;

	std::vector<Ref<libCore::ModelContainer>> modelsInScene;
	//Visible for the editor
	std::map<int, entt::entity> entitiesDictionary;
	//
	//std::map<int, libCore::ModelContainer> modelsDictionary;

	void BeginPlay();

	void Tick(float DeltaTime);


	Ref<libCore::ImportModelData> ModelData(entt::entity entity) {
		return systemRender.PrepareGeometryForImportModelData(EntityManager::GetInstance().m_registry, entity);
	}
	Ref<libCore::ModelContainer> ModelContainer(entt::entity entity) {
		return systemRender.PrepareGeometryForModelContainer(EntityManager::GetInstance().m_registry, entity);
	}
};