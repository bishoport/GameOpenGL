#pragma once
#include "../../tools/ModelLoader.h"
class RenderSystem {
public:
	Ref<libCore::ImportModelData> PrepareGeometryForImportModelData(entt::registry& registry, entt::entity& entity);
	Ref<libCore::ModelContainer> PrepareGeometryForModelContainer(entt::registry& registry, entt::entity& entity);

	void RendererBox(Ref<libCore::ModelContainer> modelContainer, glm::mat4 model, glm::mat4 camView);

	std::vector<glm::vec3> GenerateBoundingBoxLines(const glm::vec3& minBounds, const glm::vec3& maxBounds);

};