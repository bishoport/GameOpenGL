#pragma once
#include "Camera.h"
#include "../ECS/EntityManager.hpp"
class Raycast {

public:
	template <typename T> void ProjectRay(Ref<libCore::Camera> camera);

	bool rayCastIntersecting(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, glm::vec3 boxMin, glm::vec3 boxMax);
 };