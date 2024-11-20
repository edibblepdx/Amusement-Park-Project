#ifndef OBJLOADER_H
#define OBJLOADER_H

#include <stdio.h>
#include <vector>
#include <cstring>
#include <glm/glm.hpp>  // OpenGL Mathematics

static inline bool ObjLoader(const char *filename
                            , std::vector<glm::vec3> &vertices
                            , std::vector<glm::vec2> &uvs
                            , std::vector<glm::vec3> &normals)
{
    FILE * file = fopen(filename, "r");

    if (!file) 
    {
        fprintf(stderr, "Failed to open file.");
        return false;
    }

    char lineHeader[128];
    std::vector<glm::vec3> temp_vertices;
    std::vector<glm::vec2> temp_uvs;
    std::vector<glm::vec3> temp_normals;
    std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;

    // read the file
    while (fscanf(file, "%s", lineHeader) != EOF)
    {
        if (strcmp(lineHeader, "v") == 0)
        {
            // vertex
            glm::vec3 vertex;
            fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
            temp_vertices.push_back(vertex);
        }
        else if (strcmp(lineHeader, "vt") == 0)
        {
            // texture
            glm::vec2 uv;
            fscanf(file, "%f %f\n", &uv.x, &uv.y);
            temp_uvs.push_back(uv);
        }
        else if(strcmp(lineHeader, "vn") == 0)
        {
            // normal
            glm::vec3 normal;
            fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
            temp_normals.push_back(normal);
        }
        else if(strcmp(lineHeader, "f") == 0)
        {
            // face
            unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
            fscanf(file
                  , "%d/%d/%d %d/%d/%d %d/%d/%d\n"
                  , &vertexIndex[0], &uvIndex[0], &normalIndex[0]
                  , &vertexIndex[1], &uvIndex[1], &normalIndex[1]
                  , &vertexIndex[2], &uvIndex[2], &normalIndex[2]);

            vertexIndices.push_back(vertexIndex[0]);
            vertexIndices.push_back(vertexIndex[1]);
            vertexIndices.push_back(vertexIndex[2]);
            uvIndices.push_back(uvIndex[0]);
            uvIndices.push_back(uvIndex[1]);
            uvIndices.push_back(uvIndex[2]);
            normalIndices.push_back(normalIndex[0]);
            normalIndices.push_back(normalIndex[1]);
            normalIndices.push_back(normalIndex[2]);
        }
        else
        {
            // ignore rest
            char buf[1000];
            fgets(buf, 1000, file);
        }
    }// while

    // indexing
    for (unsigned int i = 0; i < vertexIndices.size(); ++i)
    {
        unsigned int vertexIndex = vertexIndices[i];
        unsigned int uvIndex = uvIndices[i];
        unsigned int normalIndex = normalIndices[i];

        glm::vec3 vertex = temp_vertices[vertexIndex - 1];
        glm::vec2 uv = temp_uvs[uvIndex - 1];
        glm::vec3 normal = temp_normals[normalIndex - 1];

        vertices.push_back(vertex);
        uvs.push_back(uv);
        normals.push_back(normal);
    }

    fclose(file);
    return true;
}

#endif // OBJLOADER_H
