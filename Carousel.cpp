/*
 * Carousel.cpp: A class that draws a carousel.
 * Hierarchical animation.
 */

#include <GL/glew.h>
#include <FL/math.h>
#include <GL/glu.h>
#include <cmath>
#include <stdio.h>
#include <iostream>
#include <math.h>
#include "Carousel.h"
#include "GenericException.h"
#include "objloader.h"
#include "libtarga.h"
//#include "TargaImage.h"

// Destructor
Carousel::~Carousel(void)
{
    if ( initialized )
    {
        glDeleteLists(track_list, 1);
        glDeleteBuffers(1, &vertexbuffer);
        glDeleteBuffers(1, &uvbuffer);
        glDeleteBuffers(1, &normalbuffer);
        glDeleteTextures(1, &texture_obj);
    }
}


// Initializer. Would return false if anything could go wrong.
bool
Carousel::Initialize(void)
{
    // make the spinning track
    GLUquadric* quad = gluNewQuadric();
    gluQuadricNormals(quad, GLU_SMOOTH);

    // Create the display list for the track
    track_list = glGenLists(1);
    glNewList(track_list, GL_COMPILE);

    // base
	glColor3f(0.2f, 0.2f, 0.2f);    // dark gray
    gluCylinder(quad, radius, radius, base_height, slices, stacks);
    glPushMatrix();
    glTranslatef(0.0, 0.0, base_height);
    gluDisk(quad, column_radius, radius, slices, stacks); // loops = stacks

    // main column
    gluCylinder(quad, column_radius, column_radius, column_height, slices, stacks);

    // horse columns
    for (int i = 0; i < num_horses; ++i)
    {
        glPushMatrix();
        glRotatef(step * i, 0.0f, 0.0f, 1.0f);
        glTranslatef(dist, 0.0f, 0.0f);
        gluCylinder(quad, 0.1, 0.1, column_height, slices, stacks);
        glPopMatrix();
    }

    //roof
	glColor3f(0.5f, 0.0f, 0.0f);    // dark red
    glTranslatef(0.0, 0.0, column_height);
    gluCylinder(quad, radius, 0, roof_height, slices, stacks);
    gluQuadricOrientation(quad, GLU_INSIDE);
    gluDisk(quad, column_radius, radius, slices, stacks); // loops = stacks
    glPopMatrix();

    glEndList();

    // Destroy the quadratics object
    gluDeleteQuadric(quad);

    // Load the horse model
    if (!ObjLoader("horse.obj", horse_vertices, horse_uvs, horse_normals))
        throw new GenericException("Carousel::C - Failed to load horse");

    // vertexbuffer
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(
        GL_ARRAY_BUFFER, 
        horse_vertices.size() * sizeof(glm::vec3), 
        horse_vertices.data(), 
        GL_STATIC_DRAW
    );

    // uvbuffer
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, 
        horse_uvs.size() * sizeof(glm::vec2), 
        horse_uvs.data(), 
        GL_STATIC_DRAW
    );

    // normalbuffer
	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, 
        horse_normals.size() * sizeof(glm::vec3), 
        horse_normals.data(), 
        GL_STATIC_DRAW
    );

    initialized = true;

    return true;
}

// Draw
void
Carousel::Draw(void)
{
    // Draw the track
    glPushMatrix();
    glRotatef(theta, 0.0f, 0.0f, 1.0f);
    glCallList(track_list);

    // Draw the horses
	// Use white, because the texture supplies the color.
    glColor3f(1.0f, 1.0f, 1.0f);

	// Turn on texturing and bind the horse texture.
	glEnable(GL_TEXTURE_2D);
    //glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_obj);
    //glTexCoordPointer(2, GL_FLOAT, 0, &horse_uvs[0]);

    //glEnable(GL_TEXTURE_2D);
    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->width, image->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image->data);
    //glBindTexture(GL_TEXTURE_2D, textureId);

    // Draw the horses
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

    for (int i = 0; i < num_horses; ++i)
    {
        glPushMatrix();
        glRotatef(step * i, 0.0f, 0.0f, 1.0f);
        if (up) glTranslatef(dist, 0.0f, horse_offset * max_horse_height / 100.0f);
        else glTranslatef(dist, 0.0f, max_horse_height - horse_offset * max_horse_height / 100.0f);
        glDrawArrays(GL_TRIANGLES, 0, horse_vertices.size());
        glPopMatrix();
    }
    
    glPopMatrix();  // translating horses upwards
    glPopMatrix();  // global rotation

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);

	// Turn texturing off
	glDisable(GL_TEXTURE_2D);
}


// Update
void
Carousel::Update(float dt)
{
    if ( ! initialized ) return;

    theta += speed * dt;
    if (theta > 360.0f) theta -= 360.0f;

    horse_offset += speed * dt * 2;
    if (horse_offset > 100.0f) 
    {
        horse_offset -= 100.0f;
        up = !up;
    }
}


