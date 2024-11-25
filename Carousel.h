/*
 * Carousel.h: Header file for a class that draws a carousel.
 * Hierarchical animation
 */

#pragma once

#include <FL/gl.h>
#include <vector>
#include <glm/glm.hpp>

class Carousel {
    private:
        GLubyte 	    track_list;	    // The display list for the track.
        bool    	    initialized;    // Whether or not we have been initialized.
        GLdouble        radius;         // Radius of the track
        GLdouble        base_height;    // Height of the base
        GLdouble        column_height;  // Height of the column
        GLdouble        roof_height;    // Height of the roof
        GLdouble        column_radius;  // Radius of the column
        GLint           slices;         // Slices of the track
        GLint           stacks;         // Stacks of the track
        GLdouble        dist;           // Distance of horses from track center
        GLint           num_horses;    // Number of horses
        GLdouble        theta;          // Rotation of track
        GLdouble        speed;          // Speed of rotation
        GLdouble        step;           // The spread of the horses on the track
        GLuint          texture_obj;    // The object for the horse texture.
        bool            up;             // up or down horses
        GLdouble        max_horse_height;   // max horse height so they don't get into roof
        GLdouble        horse_offset;   // horse offset

        // my horse model
        std::vector<glm::vec3> horse_vertices;
        std::vector<glm::vec2> horse_uvs;
        std::vector<glm::vec3> horse_normals;

        // my horse buffers
        GLuint vertexbuffer;
        GLuint uvbuffer;
        GLuint normalbuffer;

    public:
        // Constructor
        Carousel(void) { 
            initialized = false; 
            radius = 5.0f; 
            base_height = 1.0f;
            column_height = 5.0f;
            roof_height = 2.0f;
            column_radius = 0.5f;
            slices = 15; 
            stacks = 4;
            dist = radius * 2.0f / 3.0f; 
            num_horses = 5; 
            theta = 0.0f; 
            speed = 15.0f;
            step = 360.0f / num_horses;
            texture_obj = 0;
            up = true;
            max_horse_height = column_height - 4.0f;
            horse_offset = 0.0f;
        };

        // Destructor
        ~Carousel(void);

        bool    Initialize(void);	// Gets everything set up for drawing.
        void    Update(float);	// Updates the location of the horse
        void    Draw(void);		// Draws everything.
};

