#pragma once
#include "../../tools/ModelLoader.h"
class RenderSystem {
public:
	Ref<libCore::ImportModelData> PrepareGeometryForImportModelData(entt::registry& registry, entt::entity& entity);
	Ref<libCore::ModelContainer> PrepareGeometryForModelContainer(entt::registry& registry, entt::entity& entity);


};