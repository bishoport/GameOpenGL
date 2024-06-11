#pragma once
#include "../LibCoreHeaders.h"
#include <filesystem>
#include "ModelLoader.h"
#include "AssetsManager.h"
#include "../StraightSkeleton/SkeletonBuilder.h"
#include <numeric>  // Para std::iota
#include <glm/glm.hpp>  // Para transformaciones de matrices
#include <glm/gtc/matrix_transform.hpp>

namespace libCore {


    struct RoofData {
        std::vector<Vector2d> polygon;
        std::vector<Vector2d> hole;
        std::vector<std::vector<Vector2d>> holes;
        std::vector<Vector2d> expected;
    };

    class RoofGenerator {

    public:
        Skeleton BuildSkeleton(RoofData roofData) {

            roofData.expected.insert(roofData.expected.end(), roofData.polygon.begin(), roofData.polygon.end());
            roofData.expected.insert(roofData.expected.end(), roofData.hole.begin(), roofData.hole.end());

            std::cout << "Initial polygon: " << "\n";
            for (const auto& p : roofData.polygon) {
                std::cout << p.ToString() << "\n";
            }

            std::cout << "Build: " << "\n";
             //Skeleton sk = SkeletonBuilder::Build(roofData.polygon);
               Skeleton sk = SkeletonBuilder::Build(roofData.polygon,roofData.holes);

            std::cout << "Output: " << "\n";
            for (const auto& e : *sk.Distances) {
                std::cout << e.first.ToString() << "\n";
            }

            if (AssertExpectedPoints(roofData.expected, GetFacePoints(sk))) {
                std::cout << "All expected points found in the generated skeleton.\n";
            }
            else {
                std::cout << "Some expected points were not found in the generated skeleton.\n";
            }

            return sk;
        }
        

        void AddRoofVerticesFromEdges(std::vector<GLfloat>& vertexBuffer, std::vector<GLuint>& indices, const Skeleton& skeleton, size_t maxEdges = std::numeric_limits<size_t>::max()) {
            
            GLuint vertexCount = 0;
            std::map<Vector2d, size_t> vertexIndices;

            // A�adir los v�rtices de los edges del tejado
            size_t edgeCount = 0;
            for (size_t i = 0; i < skeleton.Edges->size() && edgeCount < maxEdges; ++i) {
                const auto& edgeResult = skeleton.Edges->at(i);
                if (edgeResult->Polygon) {
                    for (const auto& point : *edgeResult->Polygon) {
                        double height = skeleton.Distances->at(*point);
                        vertexBuffer.push_back(point->X);
                        vertexBuffer.push_back(height);  // Altura del punto
                        vertexBuffer.push_back(point->Y);

                        // Coordenadas UV provisionales (ser�n ajustadas por ApplyBoxUVProjection)
                        vertexBuffer.push_back(0.0f);  // UV x
                        vertexBuffer.push_back(0.0f);  // UV y

                        // Normales provisionales (puedes ajustarlas seg�n tus necesidades)
                        vertexBuffer.push_back(0.0f);
                        vertexBuffer.push_back(1.0f);
                        vertexBuffer.push_back(0.0f);

                        vertexIndices[*point] = vertexCount;
                        vertexCount++;

                        // Depuraci�n
                        //std::cout << "Vertex " << vertexCount - 1 << ": (" << point->X << ", " << height << ", " << point->Y << ")\n";
                    }
                }
                edgeCount++;
            }

            // Crear los tri�ngulos en base a la triangulaci�n de los edges
            edgeCount = 0;
            for (size_t i = 0; i < skeleton.Edges->size() && edgeCount < maxEdges; ++i) {
                const auto& edgeResult = skeleton.Edges->at(i);
                if (edgeResult->Polygon) {
                    std::vector<Vector2d> polygonPoints;
                    for (const auto& point : *edgeResult->Polygon) {
                        polygonPoints.push_back(*point);
                    }

                    std::vector<size_t> polyIndices = TriangulatePolygon(polygonPoints);

                    for (size_t idx : polyIndices) {
                        indices.push_back(vertexIndices[polygonPoints[idx]]);

                        // Depuraci�n
                        std::cout << "�ndice agregado: " << vertexIndices[polygonPoints[idx]] << "\n";
                    }
                }
                edgeCount++;
            }
        }


        Ref<ModelContainer> CreateRoof(Skeleton sk, size_t maxEdges, float uvScale = 1.0f, float uvRotation = 0.0f) {
            auto modelContainer = CreateRef<ModelContainer>();
            modelContainer->skeletal = false;

            auto modelBuild = CreateRef<Model>();
            modelContainer->name = "ROOF";

            auto mesh = CreateRef<Mesh>();

            std::vector<GLfloat> vertexBuffer;
            std::vector<GLuint> indices;
            AddRoofVerticesFromEdges(vertexBuffer, indices, sk, maxEdges);

            ApplyBoxUVProjection(vertexBuffer, uvScale, uvRotation);

            mesh->vertices.reserve(vertexBuffer.size() / 8);

            for (size_t i = 0; i < vertexBuffer.size(); i += 8) {
                Vertex vertex;
                vertex.position = glm::vec3(vertexBuffer[i], vertexBuffer[i + 1], vertexBuffer[i + 2]);
                vertex.texUV = glm::vec2(vertexBuffer[i + 3], vertexBuffer[i + 4]);
                vertex.normal = glm::vec3(vertexBuffer[i + 5], vertexBuffer[i + 6], vertexBuffer[i + 7]);

                vertex.tangent = glm::vec3(0.0f, 0.0f, 0.0f);
                vertex.bitangent = glm::vec3(0.0f, 0.0f, 0.0f);

                mesh->vertices.push_back(vertex);
            }

            mesh->indices = indices;
            mesh->SetupMesh();

            modelBuild->meshes.push_back(mesh);

            //--DEFAULT_MATERIAL
            auto material = CreateRef<Material>();
            material->materialName = "roof_material";
            material->color.r = 1.0f;
            material->color.g = 1.0f;
            material->color.b = 1.0f;
            material->albedoMap = assetsManager.GetTexture("default_roof");
            material->normalMap = assetsManager.GetTexture("default_normal");
            material->metallicMap = assetsManager.GetTexture("default_metallic");
            material->roughnessMap = assetsManager.GetTexture("default_roughness");
            material->aOMap = assetsManager.GetTexture("default_ao");
            modelBuild->materials.push_back(material);
            //----------------------------------------------------------------------------

            modelContainer->models.push_back(modelBuild);

            return modelContainer;
        }
        void ApplyBoxUVProjection(std::vector<GLfloat>& vertexBuffer, float scale = 1.0f, float rotation = 0.0f) {
            for (size_t i = 0; i < vertexBuffer.size(); i += 8) {
                float x = vertexBuffer[i];
                float y = vertexBuffer[i + 1];
                float z = vertexBuffer[i + 2];

                glm::vec3 normal = glm::vec3(vertexBuffer[i + 5], vertexBuffer[i + 6], vertexBuffer[i + 7]);
                normal = glm::normalize(normal);

                // Aplicar escala
                x *= scale;
                y *= scale;
                z *= scale;

                float u = 0.0f, v = 0.0f;

                // Proyecci�n basada en la normal de la cara
                if (fabs(normal.y) > fabs(normal.x) && fabs(normal.y) > fabs(normal.z)) {
                    // Proyecci�n plana en el plano XZ
                    u = x;
                    v = z;
                }
                else if (fabs(normal.x) > fabs(normal.z)) {
                    // Proyecci�n plana en el plano YZ
                    u = y;
                    v = z;
                }
                else {
                    // Proyecci�n plana en el plano XY
                    u = x;
                    v = y;
                }

                // Aplicar rotaci�n en el plano de proyecci�n
                if (rotation != 0.0f) {
                    float cosTheta = cos(rotation);
                    float sinTheta = sin(rotation);
                    float uRot = u * cosTheta - v * sinTheta;
                    float vRot = u * sinTheta + v * cosTheta;
                    u = uRot;
                    v = vRot;
                }

                // Actualizar coordenadas UV
                vertexBuffer[i + 3] = u;
                vertexBuffer[i + 4] = v;
            }
        }


    private:
        static bool EqualEpsilon(double d1, double d2) {
            return fabs(d1 - d2) < 5E-6;
        }
        static bool ContainsEpsilon(const std::vector<Vector2d>& list, const Vector2d& p) {
            return std::any_of(list.begin(), list.end(), [&p](const Vector2d& l) { return EqualEpsilon(l.X, p.X) && EqualEpsilon(l.Y, p.Y); });
        }
        static std::vector<Vector2d> GetFacePoints(const Skeleton& sk) {
            std::vector<Vector2d> ret;

            if (sk.Edges) {
                for (const auto& edgeOutput : *sk.Edges) {
                    auto points = edgeOutput->Polygon;
                    for (const auto& vector2d : *points) {
                        if (!ContainsEpsilon(ret, *vector2d)) {
                            ret.push_back(*vector2d);
                        }
                    }
                }
            }
            return ret;
        }
        static bool AssertExpectedPoints(const std::vector<Vector2d>& expectedList, const std::vector<Vector2d>& givenList) {
            std::stringstream sb;
            for (const Vector2d& expected : expectedList) {
                if (!ContainsEpsilon(givenList, expected))
                    sb << "Can't find expected point (" << expected.X << ", " << expected.Y << ") in given list\n";
            }

            for (const Vector2d& given : givenList) {
                if (!ContainsEpsilon(expectedList, given))
                    sb << "Can't find given point (" << given.X << ", " << given.Y << ") in expected list\n";
            }

            if (sb.tellp() > 0) {
                std::cerr << sb.str();
                return false;
            }
            else {
                return true;
            }
        }

        //HELPERS:
        bool IsCounterClockwise(const Vector2d& p1, const Vector2d& p2, const Vector2d& p3) {
            return (p2.X - p1.X) * (p3.Y - p1.Y) - (p2.Y - p1.Y) * (p3.X - p1.X) > 0;
        }
        bool IsPointInTriangle(const Vector2d& pt, const Vector2d& v1, const Vector2d& v2, const Vector2d& v3) {
            bool b1, b2, b3;
            b1 = IsCounterClockwise(pt, v1, v2);
            b2 = IsCounterClockwise(pt, v2, v3);
            b3 = IsCounterClockwise(pt, v3, v1);
            return ((b1 == b2) && (b2 == b3));
        }
        std::vector<size_t> TriangulatePolygon(const std::vector<Vector2d>& vertices) {
            std::vector<size_t> indices;
            if (vertices.size() < 3) return indices;

            std::vector<size_t> vertexIndices(vertices.size());
            std::iota(vertexIndices.begin(), vertexIndices.end(), 0);

            while (vertexIndices.size() > 3) {
                bool earFound = false;
                for (size_t i = 0; i < vertexIndices.size(); ++i) {
                    size_t prev = (i == 0) ? vertexIndices.size() - 1 : i - 1;
                    size_t next = (i == vertexIndices.size() - 1) ? 0 : i + 1;

                    const Vector2d& v0 = vertices[vertexIndices[prev]];
                    const Vector2d& v1 = vertices[vertexIndices[i]];
                    const Vector2d& v2 = vertices[vertexIndices[next]];

                    if (IsCounterClockwise(v0, v1, v2)) {
                        bool isEar = true;
                        for (size_t j = 0; j < vertexIndices.size(); ++j) {
                            if (j == prev || j == i || j == next) continue;
                            if (IsPointInTriangle(vertices[vertexIndices[j]], v0, v1, v2)) {
                                isEar = false;
                                break;
                            }
                        }

                        if (isEar) {
                            indices.push_back(vertexIndices[prev]);
                            indices.push_back(vertexIndices[i]);
                            indices.push_back(vertexIndices[next]);
                            vertexIndices.erase(vertexIndices.begin() + i);
                            earFound = true;
                            break;
                        }
                    }
                }

                if (!earFound) {
                    std::cerr << "Error: No ear found in polygon triangulation. The polygon might be non-simple or degenerate." << std::endl;
                    break;
                }
            }

            if (vertexIndices.size() == 3) {
                indices.push_back(vertexIndices[0]);
                indices.push_back(vertexIndices[1]);
                indices.push_back(vertexIndices[2]);
            }

            return indices;
        }
    };
}



























        


