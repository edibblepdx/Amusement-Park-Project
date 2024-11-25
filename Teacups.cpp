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
        //delete image;
    }
}


// Initializer. Would return false if anything could go wrong.
bool
Teacups::Initialize(void)
{
    /*
    ubyte   *image_data;
    int	    image_height, image_width;

    // Load the image for the texture. The texture file has to be in
    // a place where it will be found.
    if ( ! ( image_data = (ubyte*)tga_load("teacup_tex.tga", &image_width, &image_height, TGA_TRUECOLOR_24) ) )
    {
        fprintf(stderr, "Ground::Initialize: Couldn'teacuteacup_texd teacup_tex.tga\n");
        return false;
    }

    // This creates a texture object and binds it, so the next few operations
    // apply to this texture.
    glGenTextures(1, &texture_obj);
    glBindTexture(GL_TEXTURE_2D, texture_obj);

    // This sets a parameter for how the texture is loaded and interpreted.
    // basically, it says that the data is packed tightly in the image array.
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // This sets up the texture with high quality filtering. First it builds
    // mipmaps from the image data, then it sets the filtering parameters
    // and the wrapping parameters. We want the grass to be repeated over the
    // ground.
    gluBuild2DMipmaps(GL_TEXTURE_2D, 3, image_width, image_height, GL_RGB, GL_UNSIGNED_BYTE, image_data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);

    // This says what to do with the texture. Modulate will multiply the
    // texture by the underlying color.
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); 
    */

    glGenTextures(1, &texture_obj);
    glBindTexture(GL_TEXTURE_2D, texture_obj);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load and generate the texture
    ubyte   *image_data;
    int	    image_height, image_width;
    if ( ! ( image_data = (ubyte*)tga_load("teacup_tex.tga", &image_width, &image_height, TGA_TRUECOLOR_24) ) )
    {
        fprintf(stderr, "Ground::Initialize: Couldn't load teacup_tex.tga\n");
        return false;
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_width, image_height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_data);
    glGenerateMipmap(GL_TEXTURE_2D);

    // make the spinning track
    GLUquadric* quad = gluNewQuadric();
    gluQuadricNormals(quad, GLU_SMOOTH);

    // Create the display list for the track
    track_list = glGenLists(1);
    glNewList(track_list, GL_COMPILE);

	glColor3f(0.4f, 0.4f, 0.4f);    // dark gray
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

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // vertexbuffer
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, teacup_vertices.size() * sizeof(glm::vec3), &teacup_vertices[0], GL_STATIC_DRAW);

    // uvbuffer
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, teacup_uvs.size() * sizeof(glm::vec2), &teacup_uvs[0], GL_STATIC_DRAW);

    // normalbuffer
	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, teacup_normals.size() * sizeof(glm::vec3), &teacup_normals[0], GL_STATIC_DRAW);

    // load texture
    //LoadTexture("teacup_tex.tga");

    initialized = true;

    return true;
}

void Teacups::LoadTexture(const char* filename)
{
    /*
   image = TargaImage::Load_Image((char *)filename);
   if (!image)
   {
  	std::cerr << "Failed to load texture:  " << filename << std::endl;
  	return;
   }

   // reverse the row order
   TargaImage* reversedImage = image->Reverse_Rows();
   delete image;
   image = reversedImage;

   glGenTextures(1, &textureId);
   glBindTexture(GL_TEXTURE_2D, textureId);

   glPixelStorei(GL_UNPACK_ALIGNMENT, 1); 

   gluBuild2DMipmaps(GL_TEXTURE_2D, 3, image->width, image->height, GL_RGB, GL_UNSIGNED_BYTE, image->data);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

   glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->width, image->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image->data);
   */
}


// Draw
void
Teacups::Draw(void)
{
    /*
    // Draw the track
    //glPushMatrix();
    //glRotatef(theta, 0.0f, 0.0f, 1.0f);
    //glCallList(track_list);
    //glPopMatrix();

	// Use white, because the texture supplies the color.
    glColor3f(1.0f, 1.0f, 1.0f);

	// Turn on texturing and bind the teacup texture.
	glEnable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE0);
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

    // tell opengl how my normals are stored: floats, consecutively, zero offset.
    glNormalPointer(GL_FLOAT, 0, nullptr);

    // draw the triangles
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 1.0f);
    glDrawArrays(GL_TRIANGLES, 0, teacup_vertices.size());
    //glDrawElements(GL_TRIANGLES, teacup_vertices.size(), GL_UNSIGNED_INT, 0);
    glPopMatrix();
    
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);

	// Turn texturing off again, because we don't want everything else to be textured.
	glDisable(GL_TEXTURE_2D);
    */
}


// Update
void
Teacups::Update(float dt)
{
    if ( ! initialized ) return;

    theta += speed * dt;
    if (theta > 360.0f) theta -= 360.0f;
}


