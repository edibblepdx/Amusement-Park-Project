/*
 * Globe.cpp: A class for drawing a subdivided globe.
 */


#include <GL/glew.h>
#include <GL/glu.h>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include "Globe.h"
#include "libtarga.h"

// Destructor
Globe::~Globe(void)
{
    if ( initialized )
    {
        glDeleteTextures(1, &texture_obj);
        CleanupBuffers();
    }
}

void
Globe::CleanupBuffers()
{
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteBuffers(1, &uvbuffer);
    glDeleteBuffers(1, &normalbuffer);
    glDeleteBuffers(1, &indexbuffer);
}

void
Globe::Index()
{
    // cleanup old data
    CleanupBuffers();
    vertices.clear();
    uvs     .clear();
    normals .clear();

    for (int i = 0; i < vertex_data.size(); ++i)
    {
        vertices.push_back(vertex_data[i].pos);
        uvs     .push_back(vertex_data[i].uv);
        normals .push_back(vertex_data[i].normal);
    }

    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &uvbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);

    glGenBuffers(1, &normalbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);

    glGenBuffers(1, &indexbuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);
}

// Initializer. Returns false if something went wrong, like not being able to
// load the texture.
bool
Globe::Initialize(void)
{
    // Load textures
    ubyte   *image_data;
    int	    image_height, image_width;
    if ( ! ( image_data = (ubyte*)tga_load("2k_earth_daymap.tga", &image_width, &image_height, TGA_TRUECOLOR_24) ) )
    {
        fprintf(stderr, "Globe::Initialize: Couldn't load 2k_earth_daymap.tga\n");
        return false;
    }

    // create texture object
    glGenTextures(1, &texture_obj);
    glBindTexture(GL_TEXTURE_2D, texture_obj);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // multiply texture by underlying color
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); 

    // load and generate the texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_width, image_height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_data);
    glGenerateMipmap(GL_TEXTURE_2D);

    // free the image data
    free(image_data);

    // scale the octahedron
    for ( auto &vertex : vertex_data )
    {
        vertex.pos = radius * vertex.pos;
    }

    // index the octahedron and make buffers
    Index(); 

    // We only do all this stuff once, when the GL context is first set up.
    initialized = true;

    return true;
}


// Draw just calls the display list we set up earlier.
void
Globe::Draw(void)
{
    //glShadeModel(GL_FLAT);

    // Enable 2D texturing
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture_obj);

    // Enable client states for vertex,
    // texture coordinate,
    // normal arrays,
    // and index arrays
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);

    // Bind the vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexPointer(3, GL_FLOAT, 0, (void*)0);

    // Bind the texture coordinate buffer
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glTexCoordPointer(2, GL_FLOAT, 0, (void*)0);

    // Bind the normal buffer
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glNormalPointer(GL_FLOAT, 0, (void*)0);

    // Bind the index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexbuffer);

    // Draw the sphere
    glColor3f(1.0f, 1.0f, 1.0f); // using GL_MODULATE

    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (void*)0);

    // Disable client states
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);

    // Disable 2D texturing
    glDisable(GL_TEXTURE_2D);

    //glShadeModel(GL_SMOOTH);
}

// Update
void
Globe::Update()
{
    if ( ! initialized ) return;

    // hardcap at 5 degrees of subdivision
    degree = (degree + 1) % 6;

    // create a new octahedron and subdivide it n degrees
    std::vector<Vertex> new_data = {Octahedron_Vertices};
    std::vector<GLuint> new_indices = {};   // empty until we fill it

    // scale the initial octahedron
    for ( auto& vertex : new_data )
    {
        vertex.pos = radius * vertex.pos;
    }

    // subdivide each face n degrees
    auto indices_size = Octahedron_Indices.size();
    for ( auto i = 0; i < indices_size; i += 3 )
    {
        Subdivide(
            Octahedron_Indices[i], 
            Octahedron_Indices[i+1],
            Octahedron_Indices[i+2],
            new_data,
            new_indices,
            degree
        );
    }

    // replace old
    vertex_data = std::move(new_data);
    indices = std::move(new_indices);

    // reindex and remake buffers
    Index();
}

//          1
//        /  \
//       /    \
//     12 ---- 31
//     / \    / \
//    /   \  /   \
//   2 --- 23 --- 3

void    
Globe::Subdivide(GLuint i1, GLuint i2, GLuint i3
                , std::vector<Vertex> &vertices, std::vector<GLuint> &indices
                , GLuint degree)
{
    // Take a single face at a time (3 vertices)
    // and subdivide it n degrees. Normalize each vertex.
    // Vertices are counter-clockwise ordered.

    if (degree <= 0) 
    {
        // index and return
        indices.push_back(i1);
        indices.push_back(i2);
        indices.push_back(i3);
        return;
    }

    Vertex v1 = vertices[i1];
    Vertex v2 = vertices[i2];
    Vertex v3 = vertices[i3];

    Vertex v12, v23, v31;

    // normal
    v12.normal = glm::normalize(v1.pos + v2.pos); 
    v23.normal = glm::normalize(v2.pos + v3.pos); 
    v31.normal = glm::normalize(v3.pos + v1.pos); 

    // position
    v12.pos = radius * v12.normal;
    v23.pos = radius * v23.normal;
    v31.pos = radius * v31.normal;

    // uv
    v12.uv = (v1.uv + v2.uv) / 2.0f;
    v23.uv = (v2.uv + v3.uv) / 2.0f;
    v31.uv = (v3.uv + v1.uv) / 2.0f;

    // add new vertices
    vertices.push_back(v12);
    vertices.push_back(v23);
    vertices.push_back(v31);

    // get the indices of the new vertices
    GLuint i12 = vertices.size() - 3;
    GLuint i23 = vertices.size() - 2;
    GLuint i31 = vertices.size() - 1;

    // recurse
    Subdivide(i1, i12, i31, vertices, indices, degree - 1);
    Subdivide(i12, i2, i23, vertices, indices, degree - 1);
    Subdivide(i31, i23, i3, vertices, indices, degree - 1);
    Subdivide(i12, i23, i31, vertices, indices, degree - 1);
}
