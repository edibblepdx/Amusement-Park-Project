/*
 * CS559 Maze Project
 *
 * Class header file for the WorldWindow class. The WorldWindow is
 * the window in which the viewer's view of the world is displayed.
 *
 * (c) Stephen Chenney, University of Wisconsin at Madison, 2001-2002
 *
 */

#ifndef _WORLDWINDOW_H_
#define _WORLDWINDOW_H_

#include <FL/Fl.H>
#include <FL/Fl_Gl_Window.H>
#include <array>
#include "Ground.h"
#include "Track.h"
#include "Teacups.h"
#include "Carousel.h"
#include "Tree.h"
#include "Globe.h"
#include "Hill.h"
//#include "Horizon.h"

enum Camera {
    FREE_CAM,
    TRAIN_CAM,
};

// Subclass the Fl_Gl_Window because we want to draw OpenGL in here.
class WorldWindow : public Fl_Gl_Window {
    public:
	// Constructor takes window position and dimensions, the title.
	WorldWindow(int x, int y, int w, int h, char *label);

	// draw() method invoked whenever the view changes or the window
	// otherwise needs to be redrawn.
	void draw(void);

	// Event handling method. Uses only mouse events.
	int	handle(int);

	// Update the world according to any events that have happened since
	// the last time this method was called.
	bool Update(float);

    private:
    Camera  camera;             // The camera mode
	Ground	ground;			    // The ground object.
	Track	traintrack;	        // The train and track.
    Teacups teacups;            // The teacups object.
    Carousel carousel;          // The carousel object.
	Tree	springTree;			// A tree object.
	Tree	summerTree;			// A tree object.
	Tree	fallTree;			// A tree object.
	Tree	winterTree;			// A tree object.
    Globe   globe;              // A globe object.
    Hill    hill;               // A hill object.
	//Horizon	horizon;		// The horizon object.
    float train_pos[3], train_dir[3];   // The train position and direction.


	static const double FOV_X; // The horizontal field of view.

	float	phi;	// Viewer's inclination angle.
	float	theta;	// Viewer's azimuthal angle.
	float	dist;	// Viewer's distance from the look-at point.
	float	x_at;	// The x-coord to look at.
	float	y_at;	// The y-coord to look at. z_at is assumed 2.0.

	int     button;	// The mouse button that is down, -1 if none.
	int		x_last;	// The location of the most recent mouse event
	int		y_last;
	int		x_down; // The location of the mouse when the button was pushed
	int		y_down;
	float   phi_down;   // The view inclination angle when the mouse
			    // button was pushed
	float   theta_down; // The view azimuthal angle when the mouse
			    // button was pushed
	float	dist_down;  // The distance when the mouse button is pushed.
	float	x_at_down;  // The x-coord to look at when the mouse went down.
	float	y_at_down;  // The y-coord to look at when the mouse went down.

	void	Drag(float);	// The function to call for mouse drag events
                            
    // based on camera spawn
    // +down, +right
    // -up, -left
    std::array<float, 3 * 7> springForest {
        //bottom right
        45.0f, 45.0f, 0.0f,
        27.0f, 35.0f, 0.0f,
        37.0f, 32.0f, 0.0f,
        22.0f, 39.0f, 0.0f,

        // top right
        -22.0f, 39.0f, 0.0f,
        -27.0f, 35.0f, 0.0f,
        -37.0f, 32.0f, 0.0f,
    };

    std::array<float, 3 * 6> summerForest {
        // top left
        -44.0f, -26.0f, 0.0f,
        -33.0f, -42.0f, 0.0f,
        -36.0f, -24.0f, 0.0f,
        -24.0f, -44.0f, 0.0f,

        // top right
        -44.0f, 26.0f, 0.0f,
        -33.0f, 42.0f, 0.0f,
    };

    std::array<float, 3 * 5> fallForest {
        // bottom right
        44.0f, 30.0f, 0.0f,
        34.0f, 42.0f, 0.0f,
        24.0f, 44.0f, 0.0f,

        // top right
        -44.0f, 42.0f, 0.0f,
        43.0f, 39.0f, 0.0f,
    };

    std::array<float, 3 * 3> winterForest {
        // top left
        -45.0f, -45.0f, 0.0f,
        -27.0f, -35.0f, 0.0f,
        -37.0f, -32.0f, 0.0f,
    };
};


#endif
