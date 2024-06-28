#pragma once
#include "../tools/ModelLoader.h"
#include "ECS.h"
#include "Singleton.h"
class EntityManager : public Singleton<EntityManager> {

public:
	entt::registry m_registry;

	void GenerateEntity()
	{
		entt::entity entity = m_registry.create();

		m_registry.emplace<libCore::Transform>(entity,
			glm::vec3(0.0f),
			glm::vec3(0.0f),
			glm::vec3(1.0f)
		);
	}

	entt::entity* allTheEntities() {
		auto view = m_registry.view<>(); // Vista de todas las entidades
		std::vector<entt::entity> entitesList;
		for (auto entity : view) {
			entitesList.push_back(entity);
		}
		return entitesList.data();
	}
};