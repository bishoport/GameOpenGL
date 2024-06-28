#include "Raycast.h"

template <typename T> void Raycast::ProjectRay(Ref<libCore::Camera> camera)
{
	//float normalizedX = (2.0f * InputManager::Instance().GetMousePosition().first) / camera->width - 1.0f;
	//float normalizedY = 1.0f - (2.0f * InputManager::Instance().GetMousePosition().second) / camera->height;
	//glm::vec3 clipSpaceCoordinates(normalizedX, normalizedY, 1.0);

	//glm::vec4 homogenousCoordinates = glm::inverse(camera->cameraMatrix) * glm::vec4(clipSpaceCoordinates, 1.0);
	//glm::vec3 worldCoordinates = glm::vec3(homogenousCoordinates / homogenousCoordinates.w);


	////Preparamos el rayo para lanzarlo desde la camara hasta la posicion del mouse ya convertido al espacio 3D
	//glm::vec3 rayOrigin = camera->Position;
	//glm::vec3 rayDirection = glm::normalize(worldCoordinates - rayOrigin);

	////En esta variable se guardará el maximo valor que de un float (a modo de inicializador con valor "infinito") 
	//float closestDistance = std::numeric_limits<float>::max();
	////entt:entity[] entities = EntityManager::GetInstance().allTheEntities();

	//for (unsigned i = 0; i < entities.size(); i++) {
	//	//if(rayCastIntersecting()
	//}
}

bool Raycast::rayCastIntersecting(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, glm::vec3 boxMin, glm::vec3 boxMax)
{
	float tMin = (boxMin.x - rayOrigin.x) / rayDirection.x;
	float tMax = (boxMax.x - rayOrigin.x) / rayDirection.x;

	if (tMin > tMax) std::swap(tMin, tMax);

	float tyMin = (boxMin.y - rayOrigin.y) / rayDirection.y;
	float tyMax = (boxMax.y - rayOrigin.y) / rayDirection.y;

	if (tyMin > tyMax) std::swap(tyMin, tyMax);

	if ((tMin > tyMax) || (tyMin > tMax))
		return false;

	if (tyMin > tMin)
		tMin = tyMin;

	if (tyMax < tMax)
		tMax = tyMax;

	float tzMin = (boxMin.z - rayOrigin.z) / rayDirection.z;
	float tzMax = (boxMax.z - rayOrigin.z) / rayDirection.z;

	if (tzMin > tzMax) std::swap(tzMin, tzMax);

	if ((tMin > tzMax) || (tzMin > tMax))
		return false;

	if (tzMin > tMin)
		tMin = tzMin;

	if (tzMax < tMax)
		tMax = tzMax;

	return true;
}
