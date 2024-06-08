#pragma once
#include "entt.hpp"
namespace libCore
{
    struct ConfigComp {
        std::string name;
    };

    struct Transform {
        std::string name;
        glm::vec3 position;
        glm::vec3 rotation; // En radianes
        glm::vec3 scale;

        Transform()
            : position(0.0f, 0.0f, 0.0f),
              rotation(0.0f, 0.0f, 0.0f),
              scale(1.0f, 1.0f, 1.0f) {
        }

        Transform(glm::vec3 _pos, glm::vec3 _rotation, glm::vec3 _scale)
            : position(_pos),
            rotation(_rotation),
            scale(_scale)
        {
        
        }

        glm::mat4 getMatrix() const {
            glm::mat4 mat = glm::mat4(1.0f);
            mat = glm::translate(mat, position);
            mat = glm::rotate(mat, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
            mat = glm::rotate(mat, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
            mat = glm::rotate(mat, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
            mat = glm::scale(mat, scale);
            return mat;
        }
    };
    struct ImportModelData;
    struct ModelContainer;
    struct Renderer {

        Renderer() {

        }
        Renderer(Ref<ModelContainer> model) {

            modelContainerData = model;
        }
        Ref<ImportModelData> importModelData;
        Ref<ModelContainer> modelContainerData;
        std::string NameFile;
        std::string pathName;
    };

    struct MeshComponent {
        //VAO, VBO 
        std::vector<Ref<Mesh>> meshes;
    };

    struct MaterialComponent {
        std::vector<Ref<Material>> materials;
        /*  BaseColor
          Specular
         Metallic
         Roughness
         AmbientOcclusion*/
    };

    struct ColliderComponent {
        glm::vec3 maxBound;
        glm::vec3 minBound;
    };
}