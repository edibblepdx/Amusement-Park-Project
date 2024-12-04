/*
 * Hill.h: Header file for a class that draws the a subdivided globe.
 */


#ifndef _HILL_H_
#define _HILL_H_

#include <FL/gl.h>
#include <glm/glm.hpp>
#include <vector>
#include "Vertex.h"

// Vertices for a pyramid
const std::vector<Vertex> Pyramid_Vertices = {
    Vertex{{0, 0, 5.0},         {25, 50},     {0, 0, 2.0}},           // 0    top
    Vertex{{10.0, 10.0, 0},   {0, 25},     {10.0, 10.0, 0}},     // 1
    Vertex{{10.0, -10.0, 0},  {37.5, 25},  {10.0, -10.0, 0}},    // 2
    Vertex{{-10.0, 10.0, 0},  {25, 25},  {-10.0, 10.0, 0}},    // 10
    Vertex{{-10.0, -10.0, 0}, {25, 25},   {-10.0, -10.0, 0}},   // 4
};

// Indices for an pyramid
const std::vector<GLuint> Pyramid_Indices = {
    0, 1, 3,
    0, 3, 4,
    0, 4, 2,
    0, 2, 1,
};

class Hill {
  private:
    GLuint  texture_obj;    // The object for the grass texture.
    bool    initialized;    // Whether or not we have been initialised.

    GLuint  degree;         // The degree of subdivision.
    GLfloat scale;          // How much detail / modulation.

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
    Hill(void) { 
      initialized = false; 
      degree = 0;
      scale = 0.5f;
      vertex_data = {Pyramid_Vertices}; 
      indices = {Pyramid_Indices}, 
      vertices = {}; 
      uvs = {}; 
      normals = {}; 
    }

    ~Hill(void);

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
        
    // Splits an edge
    void    Split(Vertex&, Vertex&, Vertex&, GLfloat);
};


#endif
