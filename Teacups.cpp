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
//#include "TargaImage.h"

// Destructor
Teacups::~Teacups(void)
{
    if ( initialized )
    {
        glDeleteLists(track_list, 1);
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &vertexbuffer);
        glDeleteBuffers(1, &uvbuffer);
        glDeleteBuffers(1, &normalbuffer);
        glDeleteTextures(1, &texture_obj);
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
    gluBuild2DMipmaps(GL_TEXTURE_2D,3, image_width, image_height, GL_RGB, GL_UNSIGNED_BYTE, image_data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // multiply texture by underlying color
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); 

    // load and generate the texture
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_width, image_height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_data);
    //glGenerateMipmap(GL_TEXTURE_2D);

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
    for (auto &uv : teacup_uvs)
    {
        std::cerr << uv.x << ' ' << uv.y << std::endl;
    }
    */

    //glGenVertexArrays(1, &VAO);
    //glBindVertexArray(VAO);

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
	glBufferData(GL_ARRAY_BUFFER, 
        teacup_uvs.size() * sizeof(glm::vec2), 
        teacup_uvs.data(), 
        GL_STATIC_DRAW
    );

    // normalbuffer
	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, 
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

    // Draw the teacups
	// Use white, because the texture supplies the color.
    glColor3f(1.0f, 1.0f, 1.0f);

	// Turn on texturing and bind the teacup texture.
	glEnable(GL_TEXTURE_2D);
    //glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_obj);
    //glTexCoordPointer(2, GL_FLOAT, 0, &teacup_uvs[0]);

    //glEnable(GL_TEXTURE_2D);
    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->width, image->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image->data);
    //glBindTexture(GL_TEXTURE_2D, textureId);

    // Draw the teacups
    //glBindVertexArray(VAO);

    // 1st attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(
        0,                  // attribute
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
    );
    
    // 2nd attribute buffer : UVs
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glVertexAttribPointer(
        1,                  // attribute
        2,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
    );

    // 3rd attribute buffer : normals
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glVertexAttribPointer(
        2,                  // attribute
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
    );

    // tell opengl how my vertices, uvs, and normals are stored
    glVertexPointer(3, GL_FLOAT, 0, nullptr);
    glTexCoordPointer(2, GL_FLOAT, 0, nullptr);
    glNormalPointer(GL_FLOAT, 0, nullptr);

    // draw the triangles
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
    glPopMatrix();  // global rotation

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);

	// Turn texturing off
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


