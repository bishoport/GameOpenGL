#pragma once

#include "../LibCoreHeaders.h"
#include "../tools/PrimitivesHelper.h"
#include "../ECS/ECS.h"
#include "Mesh.h"

namespace libCore
{
    class Light
    {
    public:

        int id = 0;

        Light(): color(1.0f, 1.0f, 1.0f)
        {
            //setupDebug();
        }

        ~Light(){}

        libCore::Transform transform;

        glm::vec3 color;

        //attenuation parameters and calculate radius
        float Linear = 0.7f;
        float Quadratic = 1.8f;
        float Radius = 1.0f;

        float pointSize = 10.0f;

        Ref<Mesh> dotDebug = PrimitivesHelper::CreateDot();


        //--METHODS
        void DrawDebug(const std::string& shader)
        {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), transform.position);
            libCore::ShaderManager::Get("debug")->setMat4("model", model);
            libCore::ShaderManager::Get("debug")->setVec4("u_Color", glm::vec4(color, 1.0f));
            dotDebug->Draw();
        }

    
    };
}

