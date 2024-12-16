/*
 * Hill.cpp: A class for drawing a subdivided globe.
 */


#include <GL/glew.h>
#include <GL/glu.h>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <map>
#include "Hill.h"
#include "libtarga.h"

// Destructor
Hill::~Hill(void)
{
    if ( initialized )
    {
        glDeleteTextures(1, &texture_obj);
        CleanupBuffers();
    }
}

void
Hill::CleanupBuffers()
{
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteBuffers(1, &uvbuffer);
    glDeleteBuffers(1, &normalbuffer);
    glDeleteBuffers(1, &indexbuffer);
}

void
Hill::Index()
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
Hill::Initialize(void)
{
    // Load textures
    ubyte   *image_data;
    int	    image_height, image_width;
    if ( ! ( image_data = (ubyte*)tga_load("grass.tga", &image_width, &image_height, TGA_TRUECOLOR_24) ) )
    {
        fprintf(stderr, "Hill::Initialize: Couldn't load grass.tga\n");
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

    // index the pyramid and make buffers
    degree = 5;
    Update();
    //Index(); 

    // We only do all this stuff once, when the GL context is first set up.
    initialized = true;

    return true;
}


// Draw just calls the display list we set up earlier.
void
Hill::Draw(void)
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
Hill::Update()
{
    //if ( ! initialized ) return;

    // hardcap at 5 degrees of subdivision
    //degree = (degree + 1) % 6;

    // create a new pyramid and subdivide it n degrees
    std::vector<Vertex> new_data = {Pyramid_Vertices};
    std::vector<GLuint> new_indices = {Pyramid_Indices};   // empty until we fill it

    // subdivide each face n degrees
    auto indices_size = Pyramid_Indices.size();
    for ( auto i = 0; i < indices_size; i += 3 )
    {
        Subdivide(
            Pyramid_Indices[i], 
            Pyramid_Indices[i+1],
            Pyramid_Indices[i+2],
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
Hill::Subdivide(GLuint i1, GLuint i2, GLuint i3
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

    static std::map<std::pair<GLuint, GLuint>, GLuint> midpoint_tbl;
    float local_scale = scale * (degree + 1);

    // find a midpoint
    auto split = [&] (GLuint &i1, GLuint &i2) -> GLuint
    {
        auto it = midpoint_tbl.find(std::make_pair(std::min(i1, i2), std::max(i1, i2)));
        if (it == midpoint_tbl.end())
        {
            // create a new vertex if midpoint does not exist
            Vertex v12;
            Split(vertices[i1], vertices[i2], v12, local_scale);
            vertices.push_back(v12);

            // add to midpoint table
            GLuint index = vertices.size() - 1;
            midpoint_tbl[std::make_pair(std::min(i1, i2), std::max(i1, i2))] = index;

            return index;
        }
        return it->second;
    };

    // get the indices of the new vertices
    GLuint i12 = split(i1, i2);
    GLuint i23 = split(i2, i3);
    GLuint i31 = split(i3, i1);

    // recurse
    Subdivide(i1, i12, i31, vertices, indices, degree - 1);
    Subdivide(i12, i2, i23, vertices, indices, degree - 1);
    Subdivide(i31, i23, i3, vertices, indices, degree - 1);
    Subdivide(i12, i23, i31, vertices, indices, degree - 1);
}

void
Hill::Split(Vertex &v1, Vertex &v2, Vertex &v12, GLfloat scale)
{
    // direction vector
    glm::vec3 r = v2.pos - v1.pos;

    // position
    v12.pos = v1.pos + (r / 2.0f);

    // randomly modulate z
    // don't modulate the base
    if (v12.pos.z != 0.0f) {
        v12.pos.z += std::fmod(std::rand(), scale);
    }

    // normal
    //v12.normal = glm::normalize(v12.pos); 
    v12.normal = {0.0f, 0.0f, 1.0f};

    // uv
    v12.uv = (v1.uv + v2.uv) / 2.0f;
}
