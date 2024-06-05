#pragma once
#include "Systems/RenderSystem.h"

#include "EntityManager.hpp"

class Scene : public Singleton<Scene> {
private:
	RenderSystem systemRender;
	entt::entity entityRobot;
	entt::entity entityOperator;
	entt::entity entitySecondOperator;
	std::vector<Ref<libCore::ModelContainer>> modelsInScene;

public:
	void BeginPlay();

	void Tick(float DeltaTime);

	void TickEditor(float DeltaTime);

	Ref<libCore::ImportModelData> ModelData(entt::entity entity) {
		return systemRender.PrepareGeometryForImportModelData(EntityManager::GetInstance().m_registry, entity);
	}
	Ref<libCore::ModelContainer> ModelContainer(entt::entity entity) {
		return systemRender.PrepareGeometryForModelContainer(EntityManager::GetInstance().m_registry, entity);
	}
};