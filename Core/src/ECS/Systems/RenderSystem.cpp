#include "RenderSystem.h"

Ref<libCore::ImportModelData> RenderSystem::PrepareGeometry(entt::registry& registry, entt::entity& entity)
{
    auto view = registry.view<libCore::Renderer>();
    auto& renderer = view.get<libCore::Renderer>(entity);
    renderer.importModelData = std::shared_ptr<libCore::ImportModelData>(new libCore::ImportModelData());
    renderer.importModelData->filePath = renderer.pathName;
    renderer.importModelData->fileName = renderer.NameFile;
    renderer.importModelData->invertUV = false;
    renderer.importModelData->rotate90 = false;
    renderer.importModelData->useCustomTransform = true;
    renderer.importModelData->modelID = 1;
    renderer.importModelData->globalScaleFactor = 1.0f;
    return  renderer.importModelData;
}
