#include "PrimitivesHelper.h"


namespace libCore {

    Ref<Mesh> PrimitivesHelper::CreateQuad() {

        auto mesh = CreateRef<Mesh>();

        //mesh->vertexBuffer = {
        //    // positions        // texture Coords      // normals
        //   -0.5f,  0.5f, 0.0f,     0.0f, 0.0f,       0.0f, 0.0f, 1.0f,
        //   -0.5f, -0.5f, 0.0f,     0.0f, 0.0f,       0.0f, 0.0f, 1.0f,
        //    0.5f,  0.5f, 0.0f,     1.0f, 1.0f,       0.0f, 0.0f, 1.0f,
        //    0.5f, -0.5f, 0.0f,     1.0f, 0.0f,       0.0f, 0.0f, 1.0f,
        //};

        mesh->indices = {
            0, 1, 2,
            1, 3, 2
        };

        mesh->SetupMesh();

        return mesh;
    }

    Ref<Mesh> PrimitivesHelper::CreatePlane()
    {
        auto mesh = CreateRef<Mesh>();

        //const int divisions = 10;
        //const float size = 10.0f; // tama�o del plano completo
        //const float step = size / divisions;
        //const float halfSize = size * 0.5f;

        //for (int i = 0; i <= divisions; i++) {
        //    for (int j = 0; j <= divisions; j++) {
        //        // Posiciones
        //        mesh->vertexBuffer.push_back(j * step - halfSize); // x
        //        mesh->vertexBuffer.push_back(0.0f);                // y
        //        mesh->vertexBuffer.push_back(i * step - halfSize); // z

        //        // Coordenadas de textura
        //        mesh->vertexBuffer.push_back((float)j / divisions);  // u
        //        mesh->vertexBuffer.push_back((float)i / divisions);  // v

        //        // Normales
        //        mesh->vertexBuffer.push_back(0.0f);
        //        mesh->vertexBuffer.push_back(1.0f);
        //        mesh->vertexBuffer.push_back(0.0f);
        //    }
        //}

        //for (int i = 0; i < divisions; i++) {
        //    for (int j = 0; j < divisions; j++) {
        //        GLuint topLeft = i * (divisions + 1) + j;
        //        GLuint topRight = topLeft + 1;
        //        GLuint bottomLeft = (i + 1) * (divisions + 1) + j;
        //        GLuint bottomRight = bottomLeft + 1;

        //        mesh->indices.push_back(topLeft);
        //        mesh->indices.push_back(bottomLeft);
        //        mesh->indices.push_back(topRight);

        //        mesh->indices.push_back(topRight);
        //        mesh->indices.push_back(bottomLeft);
        //        mesh->indices.push_back(bottomRight);
        //    }
        //}
        //
        //mesh->SetupMesh();

        return mesh;
    }

    Ref<Mesh> PrimitivesHelper::CreateCube() {

        auto mesh = CreateRef<Mesh>();

        // Definici�n de v�rtices y atributos
        std::vector<GLfloat> vertexBuffer = {
            // Posiciones         // Coordenadas de Textura // Normales
            // Cara delantera
            -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,  0.0f,  0.0f,  1.0f, // V�rtice 0
            -0.5f, -0.5f,  0.5f,  0.0f, 1.0f,  0.0f,  0.0f,  1.0f, // V�rtice 1
             0.5f, -0.5f,  0.5f,  1.0f, 1.0f,  0.0f,  0.0f,  1.0f, // V�rtice 2
             0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0.0f,  0.0f,  1.0f, // V�rtice 3

             // Cara trasera
             -0.5f,  0.5f, -0.5f,  0.0f, 0.0f,  0.0f,  0.0f, -1.0f, // V�rtice 4
             -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  0.0f, -1.0f, // V�rtice 5
              0.5f, -0.5f, -0.5f,  1.0f, 1.0f,  0.0f,  0.0f, -1.0f, // V�rtice 6
              0.5f,  0.5f, -0.5f,  1.0f, 0.0f,  0.0f,  0.0f, -1.0f, // V�rtice 7

              // Cara izquierda
              -0.5f,  0.5f, -0.5f,  1.0f, 0.0f, -1.0f,  0.0f,  0.0f, // V�rtice 8
              -0.5f, -0.5f, -0.5f,  1.0f, 1.0f, -1.0f,  0.0f,  0.0f, // V�rtice 9
              -0.5f, -0.5f,  0.5f,  0.0f, 1.0f, -1.0f,  0.0f,  0.0f, // V�rtice 10
              -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, -1.0f,  0.0f,  0.0f, // V�rtice 11

              // Cara derecha
               0.5f,  0.5f, -0.5f,  1.0f, 0.0f,  1.0f,  0.0f,  0.0f, // V�rtice 12
               0.5f, -0.5f, -0.5f,  1.0f, 1.0f,  1.0f,  0.0f,  0.0f, // V�rtice 13
               0.5f, -0.5f,  0.5f,  0.0f, 1.0f,  1.0f,  0.0f,  0.0f, // V�rtice 14
               0.5f,  0.5f,  0.5f,  0.0f, 0.0f,  1.0f,  0.0f,  0.0f, // V�rtice 15

               // Cara superior
               -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  1.0f,  0.0f, // V�rtice 16
               -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,  0.0f,  1.0f,  0.0f, // V�rtice 17
                0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0.0f,  1.0f,  0.0f, // V�rtice 18
                0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f,  1.0f,  0.0f, // V�rtice 19

                // Cara inferior
                -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f, -1.0f,  0.0f, // V�rtice 20
                -0.5f, -0.5f,  0.5f,  0.0f, 1.0f,  0.0f, -1.0f,  0.0f, // V�rtice 21
                 0.5f, -0.5f,  0.5f,  1.0f, 1.0f,  0.0f, -1.0f,  0.0f, // V�rtice 22
                 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  0.0f, -1.0f,  0.0f  // V�rtice 23
        };

        std::vector<GLuint> indices = {
            // Cara delantera
            0, 1, 2, 0, 2, 3,
            // Cara trasera
            4, 5, 6, 4, 6, 7,
            // Cara izquierda
            8, 9, 10, 8, 10, 11,
            // Cara derecha
            12, 13, 14, 12, 14, 15,
            // Cara superior
            16, 17, 18, 16, 18, 19,
            // Cara inferior
            20, 21, 22, 20, 22, 23
        };

        mesh->vertices.reserve(vertexBuffer.size() / 8); // Dividimos por 8 porque cada v�rtice tiene 8 floats

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

        return mesh;
    }

    Ref<Mesh> PrimitivesHelper::CreateSphere(float radius, unsigned int sectorCount, unsigned int stackCount) {

        auto mesh = CreateRef<Mesh>();

        const float PI = 3.14159265359f;

        for (unsigned int x = 0; x <= sectorCount; ++x)
        {
            for (unsigned int y = 0; y <= stackCount; ++y)
            {
                Vertex vertex;

                float xSegment = (float)x / (float)sectorCount;
                float ySegment = (float)y / (float)stackCount;
                float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
                float yPos = std::cos(ySegment * PI);
                float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

                vertex.position = glm::vec3(xPos, yPos, zPos) * radius;
                vertex.texUV = glm::vec2(xSegment, ySegment);
                vertex.normal = glm::vec3(xPos, yPos, zPos);

                vertex.tangent = glm::vec3(0.0f, 0.0f, 0.0f);
                vertex.bitangent = glm::vec3(0.0f, 0.0f, 0.0f);

                mesh->vertices.push_back(vertex);
            }
        }

        for (unsigned int y = 0; y < stackCount; ++y)
        {
            for (unsigned int x = 0; x <= sectorCount; ++x)
            {
                mesh->indices.push_back(y * (sectorCount + 1) + x);
                mesh->indices.push_back((y + 1) * (sectorCount + 1) + x);
                mesh->indices.push_back((y + 1) * (sectorCount + 1) + x + 1);
                mesh->indices.push_back(y * (sectorCount + 1) + x);
                mesh->indices.push_back((y + 1) * (sectorCount + 1) + x + 1);
                mesh->indices.push_back(y * (sectorCount + 1) + x + 1);
            }
        }


        mesh->SetupMesh();

        return mesh;
    }
    
    Ref<Mesh> PrimitivesHelper::CreateDot() {
        auto mesh = CreateRef<Mesh>();

        // Definiendo un punto
        std::vector<GLfloat> vertexBuffer = {
            0.0f, 0.0f, 0.0f, // Posici�n
            0.0f, 0.0f,       // TexUV
            0.0f, 0.0f, 0.0f  // Normal
        };

        // Definiendo el �ndice
        mesh->indices = { 0 };

        mesh->vertices.reserve(1); // Solo necesitamos reservar espacio para un v�rtice

        // Creaci�n del v�rtice
        Vertex vertex;
        vertex.position = glm::vec3(vertexBuffer[0], vertexBuffer[1], vertexBuffer[2]);
        vertex.texUV = glm::vec2(vertexBuffer[3], vertexBuffer[4]);
        vertex.normal = glm::vec3(vertexBuffer[5], vertexBuffer[6], vertexBuffer[7]);

        vertex.tangent = glm::vec3(0.0f, 0.0f, 0.0f);
        vertex.bitangent = glm::vec3(0.0f, 0.0f, 0.0f);

        mesh->vertices.push_back(vertex);

        mesh->drawLike = DRAW_GEOM_LIKE::DOT;
        mesh->SetupMesh();

        return mesh;
    }

    Ref<Mesh> PrimitivesHelper::CreateLine(const glm::vec3& point1, const glm::vec3& point2)
    {
        auto mesh = CreateRef<Mesh>();

        // Definiendo dos puntos para la l�nea usando los par�metros
        std::vector<GLfloat> vertexBuffer = {
            // Primer punto
            point1.x, point1.y, point1.z, // Posici�n
            0.0f, 0.0f,                   // TexUV
            0.0f, 0.0f, 0.0f,             // Normal

            // Segundo punto
            point2.x, point2.y, point2.z, // Posici�n
            0.0f, 0.0f,                   // TexUV
            0.0f, 0.0f, 0.0f              // Normal
        };

        // Definiendo los �ndices
        mesh->indices = { 0, 1 };

        mesh->vertices.reserve(2); // Reservamos espacio para dos v�rtices

        for (size_t i = 0; i < vertexBuffer.size(); i += 8) {
            Vertex vertex;
            vertex.position = glm::vec3(vertexBuffer[i], vertexBuffer[i + 1], vertexBuffer[i + 2]);
            vertex.texUV = glm::vec2(vertexBuffer[i + 3], vertexBuffer[i + 4]);
            vertex.normal = glm::vec3(vertexBuffer[i + 5], vertexBuffer[i + 6], vertexBuffer[i + 7]);

            vertex.tangent = glm::vec3(0.0f, 0.0f, 0.0f);
            vertex.bitangent = glm::vec3(0.0f, 0.0f, 0.0f);

            mesh->vertices.push_back(vertex);
        }
        mesh->drawLike = DRAW_GEOM_LIKE::LINE;
        mesh->SetupMesh();

        return mesh;
    }
}