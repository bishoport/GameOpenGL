#pragma once
#include "entt.hpp"
#include <glm/gtx/matrix_decompose.hpp>
namespace libCore
{
    struct EntityInfo {
        std::string name;
        //tag
        unsigned indexTag;
        // layer
        unsigned indexLayer;
    };

    struct Transform {
        std::string name;
        glm::vec3 position;
        glm::vec3 rotation; // En radianes
        glm::vec3 scale;
        glm::vec3 direction = glm::vec3(1.0f, 1.0f, 1.0f);
        glm::quat quaternion;

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
       /* void setMatrix(const glm::mat4& matrix) {
            glm::vec3 skew;
            glm::vec4 perspective;
            glm::quat quaternion;
            glm::decompose(matrix, scale, quaternion, position, skew, perspective);
            rotation = glm::eulerAngles(quaternion);
            std::cout << "ROTATION " << std::endl;
        }*/

        void setMatrix(const glm::mat4& matrix) {
            glm::vec3 newPos = glm::vec3(matrix[3]);

            glm::vec3 rows[3];

            for (int i = 0; i < 3; i++) {
                rows[i] = glm::vec3(matrix[i]);
            }
            glm::vec3 newScale;
            newScale.x = glm::length(rows[0]);
            newScale.y = glm::length(rows[1]);
            newScale.z = glm::length(rows[2]);

            // Quitar la escala de la matriz de transformación
            if (newScale.x) rows[0] /= newScale.x;
            if (newScale.y) rows[1] /= newScale.y;
            if (newScale.z) rows[2] /= newScale.z;

            // Extraer la rotación
            glm::mat3 rotationMatrix(
                rows[0][0], rows[0][1], rows[0][2],
                rows[1][0], rows[1][1], rows[1][2],
                rows[2][0], rows[2][1], rows[2][2]
            );

          //  glm::vec3 baseVector = glm::vec3(1.0f, 0.0f, 0.0f); // Vector en la dirección X
            glm::quat rotationQuat = glm::quat_cast(rotationMatrix);
            rotationQuat = reeestructureQuaternion(rotationQuat);
            glm::quat restQuat = rotationQuat - quaternion;
            std::cout << "OLO" << std::endl;
            if (rotationQuat != quaternion) {
                glm::vec3 newRotation = glm::eulerAngles(rotationQuat);
                quaternion = rotationQuat;
                rotation = newRotation;
            }
        

            position = newPos;


            scale = newScale;
        }
       
        glm::quat reeestructureQuaternion(glm::quat quat) {
            quat.x = std::floor(quat.x * 100) / 100;
            quat.y = std::floor(quat.y * 100) / 100;
            quat.z = std::floor(quat.z * 100) / 100;
            quat.w = std::floor(quat.w * 100) / 100;
            return quat;
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