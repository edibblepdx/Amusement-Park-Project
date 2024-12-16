/*
 * Globe.h: Header file for a class that draws the a subdivided globe.
 */


#ifndef _GLOBE_H_
#define _GLOBE_H_

#include <FL/gl.h>
#include <glm/glm.hpp>
#include <vector>
#include "Vertex.h"

// Vertices for an octahedron
const std::vector<Vertex> Octahedron_Vertices = {
    Vertex{{0, 0, 1},         {0.5, 1},     {0, 0, 1}},           // 0    top
    Vertex{{0, 0, -1},        {0.5, 0},     {0, 0, -1}},          // 1    bottom
    Vertex{{0.71, 0.71, 0},   {0, 0.5},     {0.71, 0.71, 0}},     // 2
    Vertex{{0.71, -0.71, 0},  {0.75, 0.5},  {0.71, -0.71, 0}},    // 3
    Vertex{{-0.71, 0.71, 0},  {0.25, 0.5},  {-0.71, 0.71, 0}},    // 4
    Vertex{{-0.71, -0.71, 0}, {0.5, 0.5},   {-0.71, -0.71, 0}},   // 5
};

// Indices for an octahedron
const std::vector<GLuint> Octahedron_Indices = {
    0, 2, 4,
    0, 4, 5,
    0, 5, 3,
    0, 3, 2,
    1, 4, 2,
    1, 5, 4,
    1, 3, 5,
    1, 2, 3,
};

class Globe {
  private:
    GLuint  texture_obj;    // The object for the grass texture.
    bool    initialized;    // Whether or not we have been initialised.

    GLuint  degree;         // The degree of subdivision.
    GLfloat radius;         // The radius of the globe.

    // globe data
    std::vector<Vertex> vertex_data;  // each element contains pos, uv, normal
    std::vector<GLuint> indices;   
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;

    // globe buffers
    GLuint vertexbuffer;
    GLuint uvbuffer;
    GLuint normalbuffer;
    GLuint indexbuffer;

  public:
    Globe(void) { 
      initialized = false; 
      degree = 0;
      radius = 10.0;
      vertex_data = {Octahedron_Vertices}; 
      indices = {Octahedron_Indices}, 
      vertices = {}; 
      uvs = {}; 
      normals = {}; 
    }

    ~Globe(void);

    void    CleanupBuffers(void);

    void    Index();

    // Initializer. Creates the display list.
    bool    Initialize(void);

    // Does the drawing.
    void    Draw(void);

    // Wraps the subdivision
    void    Update();

    // Does the subdivision
    void    Subdivide(GLuint, GLuint, GLuint, std::vector<Vertex>&, std::vector<GLuint>&, GLuint);
};


#endif
