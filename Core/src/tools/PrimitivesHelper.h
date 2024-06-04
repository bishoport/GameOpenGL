#pragma once

#include "../LibCoreHeaders.h"
#include "../Core/Mesh.h"

namespace libCore 
{
    class PrimitivesHelper 
    {

    public:
        static Ref<Mesh> CreateQuad();
        static Ref<Mesh> CreatePlane();
        static Ref<Mesh> CreateCube();
        static Ref<Mesh> CreateSphere(float radius, unsigned int sectorCount, unsigned int stackCount);
        static Ref<Mesh> CreateDot();
        static Ref<Mesh> CreateLine(const glm::vec3& point1, const glm::vec3& point2);
    };
}