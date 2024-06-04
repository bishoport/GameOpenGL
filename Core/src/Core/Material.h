#pragma once

#include "../LibCoreHeaders.h"
#include <string>
#include "Texture.h"

namespace libCore
{
    class Material
    {
    public:
        std::string materialName = "none";

        std::string shaderName = "default";

        //VALUES
        glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

        //TEXTURES
        Ref<Texture> albedoMap;
        Ref<Texture> normalMap;
        Ref<Texture> metallicMap;
        Ref<Texture> rougnessMap;
        Ref<Texture> aOMap;
        //-------------------------------------------------------
    };
}