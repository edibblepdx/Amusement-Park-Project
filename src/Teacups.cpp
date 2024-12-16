/*
 * Teacups.cpp: A class that draws rotating teacups.
 * Hierarchical animation.
 */

#include <GL/glew.h>
#include <FL/math.h>
#include <GL/glu.h>
#include <cmath>
#include <stdio.h>
#include <iostream>
#include <math.h>
#include "Teacups.h"
#include "GenericException.h"
#include "objloader.h"
#include "libtarga.h"

// Destructor
Teacups::~Teacups(void)
{
    if ( initialized )
    {
        glDeleteLists(track_list, 1);
        glDeleteTextures(1, &texture_obj);
        glDeleteBuffers(1, &vertexbuffer);
        glDeleteBuffers(1, &uvbuffer);
        glDeleteBuffers(1, &normalbuffer);
    }
}


// Initializer. Would return false if anything could go wrong.
bool
Teacups::Initialize(void)
{
    // Load textures
    ubyte   *image_data;
    int	    image_height, image_width;
    if ( ! ( image_data = (ubyte*)tga_load("teacup_tex.tga", &image_width, &image_height, TGA_TRUECOLOR_24) ) )
    {
        fprintf(stderr, "Ground::Initialize: Couldn't load teacup_tex.tga\n");
        return false;
    }

    // create texture object
    glGenTextures(1, &texture_obj);
    glBindTexture(GL_TEXTURE_2D, texture_obj);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // set the texture wrapping/filtering options (on the currently bound texture object)
    //gluBuild2DMipmaps(GL_TEXTURE_2D,3, image_width, image_height, GL_RGB, GL_UNSIGNED_BYTE, image_data);
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

    // make the spinning track
    GLUquadric* quad = gluNewQuadric();
    gluQuadricNormals(quad, GLU_SMOOTH);

    // Create the display list for the track
    track_list = glGenLists(1);
    glNewList(track_list, GL_COMPILE);

	glColor3f(0.2f, 0.2f, 0.2f);    // dark gray
    gluCylinder(quad, radius, radius, height, slices, stacks);
    glPushMatrix();
    glTranslatef(0.0, 0.0, height);
    gluDisk(quad, 0, radius, slices, stacks); // loops = stacks
    glPopMatrix();

    glEndList();

    // Destroy the quadratics object
    gluDeleteQuadric(quad);

    // Load the teacup model
    if (!ObjLoader("teacup_car.obj", teacup_vertices, teacup_uvs, teacup_normals))
        throw new GenericException("Teacups::C - Failed to load teacup");

    /*
    // verify uvs are correct: they are all [0, 1]
    for (auto &uv : teacup_uvs)
    {
        std::cerr << uv.x << ' ' << uv.y << std::endl;
    }
    // verify all sizes are the same: they are
    std::cerr << teacup_vertices.size() << ' ' << teacup_uvs.size() << ' ' << teacup_normals.size() << std::endl;
    */

    // Binding to a VAO is breaking the object
    //glGenVertexArrays(1, &VAO);
    //glBindVertexArray(VAO);

    // The VAO should allow me to do
    // glEnableVertexAttribArray(); and glVertexAttribPointer(); calls once during initialization
    // instead of every Draw() call. The buffers would be bound to the VAO then I just bind the VAO 
    // in the Draw() function and call glDrawElements() instead of glDrawArrays() but I believe this
    // breaking because I have no shaders so instead I have to used the fixed function pipeline
    // and call glVertexPointer(3, GL_FLOAT, 0, nullptr); glTexCoordPointer(2, GL_FLOAT, 0, nullptr);
    // glNormalPointer(GL_FLOAT, 0, nullptr); to specify how the objects are stored followed by a call
    // to glDrawArrays(). OpenGL implicitly binds these to a vertex array object each time anyway, but 
    // it would just be more efficient to make my own and the glDeleteVertexArrays() would also do the 
    // cleanup. I just can't figure out how to do it properly without a broken object. Our lighting 
    // simple system is also causing issues. The nullptr in the previous function calls just makes
    // openGL look at the currently bound GL_ARRAY_BUFFER instead of another array.

    // vertexbuffer
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(
        GL_ARRAY_BUFFER, 
        teacup_vertices.size() * sizeof(glm::vec3), 
        teacup_vertices.data(), 
        GL_STATIC_DRAW
    );

    // uvbuffer
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(
        GL_ARRAY_BUFFER, 
        teacup_uvs.size() * sizeof(glm::vec2), 
        teacup_uvs.data(), 
        GL_STATIC_DRAW
    );

    // normalbuffer
	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(
        GL_ARRAY_BUFFER, 
        teacup_normals.size() * sizeof(glm::vec3), 
        teacup_normals.data(), 
        GL_STATIC_DRAW
    );

    initialized = true;

    return true;
}

// Draw
void
Teacups::Draw(void)
{
    // Draw the track
    glPushMatrix();
    glRotatef(theta, 0.0f, 0.0f, 1.0f);
    glCallList(track_list);

    // Draw the teacups VAO is breaking
    //glBindVertexArray(VAO);
    
    // If I could get the VAO working glEnableVertexAttribArray(); and glVertexAttribPointer();
    // would only need to be done once in Initialize();

    // Draw the teacups
	// Use white, because the texture supplies the color.
    glColor3f(1.0f, 1.0f, 1.0f);

    // Enable 2D texturing
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture_obj);

    // Enable client states for vertex,
    // texture coordinate,
    // and normal arrays
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

    // Draw the teacups
    glColor3f(1.0f, 1.0f, 1.0f); // using GL_MODULATE

    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 1.0f);

    for (int i = 0; i < num_teacups; ++i)
    {
        glPushMatrix();
        glRotatef(step * i, 0.0f, 0.0f, 1.0f);
        glTranslatef(dist, 0.0f, 0.0f);
        glRotatef(theta * 3, 0.0f, 0.0f, 1.0f);
        glDrawArrays(GL_TRIANGLES, 0, teacup_vertices.size());
        glPopMatrix();
    }
    
    glPopMatrix();  // translating teacups upwards
    glPopMatrix();  // global object rotation

    // Disable client states
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);

    // Disable 2D texturing
    glDisable(GL_TEXTURE_2D);
}


// Update
void
Teacups::Update(float dt)
{
    if ( ! initialized ) return;

    theta += speed * dt;
    if (theta > 360.0f) theta -= 360.0f;
}


