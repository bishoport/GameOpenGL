#pragma once
#include "EntityManager.hpp"
#include "Systems/RenderSystem.h"

class Scene {
private:
	RenderSystem* systemRender;
	entt::entity entityRobot;
	entt::entity entityOperator;
	entt::entity entitySecondOperator;
public:
	void BeginPlay();

	void Tick(float DeltaTime);

	//Ref<libCore::ImportModelData> ModelData(entt::entity entity) {
	//	return systemRender.PrepareGeometry(EntityManager::GetInstance().m_registry, entity);
	//}

};