#pragma once
#include "../../Core/EngineOpenGL.h"
class RenderSystem {
public:
	Ref<libCore::ImportModelData> PrepareGeometry(entt::registry& registry, entt::entity& entity);

};