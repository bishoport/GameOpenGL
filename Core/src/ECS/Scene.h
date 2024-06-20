#pragma once
#include "Systems/RenderSystem.h"

#include "EntityManager.hpp"

class Scene : public Singleton<Scene> {
private:
	
	entt::entity EntityCreator(Ref<libCore::ModelContainer> modelContainer, std::string nameEntity = "");

	void CreateTagsAndLayers();

	std::string const jsonFile = "C:/Users/migue/Desktop/Projects/GameOpenGL/Core/assets/data/tagsAndLayers.json";
	

public:

	std::vector<std::string> tags;
	std::vector<std::string> layers;

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

	void CreateCube();
	void CreateSphere(float radius = 6, unsigned int sectorCount  = 1 , unsigned int stackCount = 1);


};