/*
 * Track.h: Header file for a class that draws the train and its track.
 *
 * (c) 2001: Stephen Chenney, University of Wisconsin at Madison.
 */


#ifndef _TRAINTRACK_H_
#define _TRAINTRACK_H_

#include <FL/gl.h>
#include <vector>
#include <glm/glm.hpp>
#include "CubicBspline.h"

class Track {
  private:
    GLubyte 	    track_list;	    // The display list for the track.
    GLubyte 	    train_list;	    // The display list for the train.
    bool    	    initialized;    // Whether or not we have been initialized.
    CubicBspline    *track;	        // The spline that defines the track.
    float	        posn_on_track;  // The train's parametric position on the track.
    float	        speed;	        // The train's speed, in world coordinates

    static const int	TRACK_NUM_CONTROLS;	// Constants about the track.
    static const float 	TRACK_CONTROLS[][3];
    static const float 	TRAIN_ENERGY;

    // my train model
    std::vector<glm::vec3> train_vertices;
    std::vector<glm::vec2> train_uvs;
    std::vector<glm::vec3> train_normals;

    // my train buffers
    GLuint vertexbuffer;
    GLuint uvbuffer;
    GLuint normalbuffer;

    GLuint          texture_obj;    // The object for the teacup texture.

  public:
    // Constructor
    Track(void) { initialized = false; posn_on_track = 0.0f; speed = 0.0f; texture_obj = 0; };

    // Destructor
    ~Track(void);

    bool    Initialize(void);	// Gets everything set up for drawing.
    void    Update(float, float*, float*);	// Updates the location of the train
    void    Draw(void);		// Draws everything.
};


#endif

