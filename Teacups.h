/*
 * Teacups.h: Header file for a class that draws rotating teacups
 * Hierarchical animation
 */

#pragma once

#include <FL/gl.h>
#include <vector>
#include <glm/glm.hpp>
//#include "TargaImage.h"

class Teacups {
    private:
        GLubyte 	    track_list;	    // The display list for the track.
        bool    	    initialized;    // Whether or not we have been initialized.
        GLdouble        radius;         // Radius of the track
        GLdouble        height;         // Height of the track
        GLint           slices;         // Slices of the track
        GLint           stacks;         // Stacks of the track
        GLdouble        dist;           // Distance of teacups from track center
        GLint           num_teacups;    // Number of teacups
        GLdouble        theta;          // Rotation of track
        GLdouble        speed;          // Speed of rotation
        GLdouble        step;           // The spread of the teacups on the track
        //unsigned int    textureId;      // Teacup texture id
        //TargaImage      *image;
        GLuint          texture_obj;    // The object for the teacup texture.

        // my teacup model
        std::vector<glm::vec3> teacup_vertices;
        std::vector<glm::vec2> teacup_uvs;
        std::vector<glm::vec3> teacup_normals;

        // my teacup buffers
        GLuint VAO;
        GLuint vertexbuffer;
        GLuint uvbuffer;
        GLuint normalbuffer;

    public:
        // Constructor
        Teacups(void) { 
            initialized = false; 
            radius = 10.0f; 
            height = 1.0f; 
            slices = 10; 
            stacks = 4;
            dist = radius / 2.0f; 
            num_teacups = 3; 
            theta = 0.0f; 
            speed = 15.0f;
            step = 360.0f / num_teacups;
            //textureId = 0;
            //image = nullptr;
            texture_obj = 0;
        };

        // Destructor
        ~Teacups(void);

        bool    Initialize(void);	// Gets everything set up for drawing.
        void    LoadTexture(const char*);    // Load teacup texture
        void    Update(float);	// Updates the location of the teacup
        void    Draw(void);		// Draws everything.
};
