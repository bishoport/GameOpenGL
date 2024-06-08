#include "RenderSystem.h"

Ref<libCore::ImportModelData> RenderSystem::PrepareGeometryForImportModelData(entt::registry& registry, entt::entity& entity)
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

Ref<libCore::ModelContainer> RenderSystem::PrepareGeometryForModelContainer(entt::registry& registry, entt::entity& entity)
{
    auto view = registry.view<libCore::Renderer>();
    auto& renderer = view.get<libCore::Renderer>(entity);
    renderer.modelContainerData = std::shared_ptr<libCore::ModelContainer>(new libCore::ModelContainer());
    renderer.modelContainerData->name = renderer.NameFile;
  
    return renderer.modelContainerData;
}

void RenderSystem::RendererBox(Ref<libCore::ModelContainer> modelContainer, glm::mat4 model, glm::mat4 camView)
{
	glm::vec3 boundingBoxMin = glm::vec3(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
	glm::vec3 boundingBoxMax = glm::vec3(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());

	for (unsigned int i = 0; i < modelContainer->models.size(); i += 1)
	{
		for (unsigned int j = 0; j < modelContainer->models[i]->meshes.size(); j += 1) {
			for (unsigned int h = 0; h < modelContainer->models[i]->meshes[j]->vertices.size(); h += 1) {
				GLfloat x = modelContainer->models[i]->meshes[j]->vertices[h].position.x;
				GLfloat y = modelContainer->models[i]->meshes[j]->vertices[h].position.y;
				GLfloat z = modelContainer->models[i]->meshes[j]->vertices[h].position.z;
				glm::vec4 vertex = model * glm::vec4(x, y, z, 1.f);
				// Update the minimum and maximum values of the bounding box
				boundingBoxMin.x = std::min(boundingBoxMin.x, vertex.x);
				boundingBoxMin.y = std::min(boundingBoxMin.y, vertex.y);
				boundingBoxMin.z = std::min(boundingBoxMin.z, vertex.z);

				boundingBoxMax.x = std::max(boundingBoxMax.x, vertex.x);
				boundingBoxMax.y = std::max(boundingBoxMax.y, vertex.y);
				boundingBoxMax.z = std::max(boundingBoxMax.z, vertex.z);
			}
		
		}

	}

	std::vector<glm::vec3> lines = GenerateBoundingBoxLines(boundingBoxMin, boundingBoxMax);

}

std::vector<glm::vec3> RenderSystem::GenerateBoundingBoxLines(const glm::vec3& minBounds, const glm::vec3& maxBounds)
{
	{
		// Generamos los 8 vértices de la bounding box
		std::vector<glm::vec3> vertices = {
			{minBounds.x, minBounds.y, minBounds.z},
			{maxBounds.x, minBounds.y, minBounds.z},
			{maxBounds.x, minBounds.y, maxBounds.z},
			{minBounds.x, minBounds.y, maxBounds.z},
			{minBounds.x, maxBounds.y, minBounds.z},
			{maxBounds.x, maxBounds.y, minBounds.z},
			{maxBounds.x, maxBounds.y, maxBounds.z},
			{minBounds.x, maxBounds.y, maxBounds.z},
		};

		// Usamos los vértices para crear las líneas de la bounding box (cada línea es definida por 2 vértices)
		std::vector<glm::vec3> lines = {
			vertices[0], vertices[1], vertices[1], vertices[2], vertices[2], vertices[3], vertices[3], vertices[0], // Abajo
			vertices[4], vertices[5], vertices[5], vertices[6], vertices[6], vertices[7], vertices[7], vertices[4], // Arriba
			vertices[0], vertices[4], vertices[1], vertices[5], vertices[2], vertices[6], vertices[3], vertices[7], // Laterales
		};

		return lines;
	}
}

