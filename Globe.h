/*
 * Globe.h: Header file for a class that draws the a subdivided globe.
 */


#ifndef _GLOBE_H_
#define _GLOBE_H_

#include <FL/gl.h>
#include <glm/glm.hpp>
#include <vector>

struct Vertex {
    glm::vec3 pos;
    glm::vec2 uv;
    glm::vec3 normal;
};

// Vertices for an octahedron
const std::vector<Vertex> Octahedron_Vertices = {
    Vertex{{0, 0, 1}, {0.5, 1}, {0, 0, 1}},                   // 0    top
    Vertex{{0, 0, -1}, {0.5, 0}, {0, 0, -1}},                 // 1    bottom
    Vertex{{0.71, 0.71, 0}, {0, 0.5}, {0.71, 0.71, 0}},       // 2
    Vertex{{0.71, -0.71, 0}, {0.75, 0.5}, {0.71, -0.71, 0}},     // 3
    Vertex{{-0.71, 0.71, 0}, {0.25, 0.5}, {-0.71, 0.71, 0}},     // 4
    Vertex{{-0.71, -0.71, 0}, {0.5, 0.5}, {-0.71, -0.71, 0}},   // 5
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
    GLubyte display_list;   // The display list that does all the work.
    GLuint  texture_obj;    // The object for the grass texture.
    bool    initialized;    // Whether or not we have been initialised.

    std::vector<Vertex> vertex_data;
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
      display_list = 0; 
      initialized = false; 
      vertex_data = {Octahedron_Vertices}; 
      indices = {Octahedron_Indices}, 
      vertices = {}; 
      uvs = {}; 
      normals = {}; 
    }

    ~Globe(void);

    void    Index();

    // Initializer. Creates the display list.
    bool    Initialize(void);

    // Does the drawing.
    void    Draw(void);
};


#endif
