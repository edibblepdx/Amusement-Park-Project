/*
 * CS559 Maze Project
 *
 * Class file for the WorldWindow class.
 *
 * (c) Stephen Chenney, University of Wisconsin at Madison, 2001-2002
 *
 */

#include <stdio.h>
#include <GL/glew.h>
#include <FL/math.h>
#include <FL/gl.h>
#include <GL/glu.h>
#include "WorldWindow.h"

const double WorldWindow::FOV_X = 45.0;

WorldWindow::WorldWindow(int x, int y, int width, int height, char *label)
: Fl_Gl_Window(x, y, width, height, label)
, ground{}
, traintrack{}
, teacups{}
, carousel{}
, globe{}
, hill{}
, springTree{SPRING, 2.0f, 0.25f, 8.0f, 2.0f}
, summerTree{SUMMER, 1.75f, 1.0f, 6.0f, 3.5f}
, fallTree{FALL, 3.0f, 0.5f, 5.0f, 2.0f}
, winterTree{WINTER, 4.0f, 0.5f, 7.0f, 3.0f}
//, horizon{}
{
    button = -1;

    train_pos[0] = train_pos[1] = train_pos[2] = 0.0f;
    train_dir[0] = train_dir[1] = train_dir[2] = 0.0f; 

    camera = FREE_CAM;
    // Initial viewing parameters.
    phi = 45.0f;
    theta = 0.0f;
    dist = 100.0f;
    x_at = 0.0f;
    y_at = 0.0f;

}


void
WorldWindow::draw(void)
{
    double  eye[3];
    float   color[4], dir[4];

    if ( ! valid() )
    {
        // Stuff in here doesn't change from frame to frame, and does not
        // rely on any coordinate system. It only has to be done if the
        // GL context is damaged.

        // initialize glew (for models)
        GLenum err = glewInit();
        if (GLEW_OK != err)
        {
            /* Problem: glewInit failed, something is seriously wrong. */
            fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
        }

        double	fov_y;

        // Sets the clear color to sky blue.
        glClearColor(0.53f, 0.81f, 0.92f, 1.0);

        // Turn on depth testing
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        // Turn on back face culling. Faces with normals away from the viewer
        // will not be drawn.
        glEnable(GL_CULL_FACE);

        // Enable lighting with one light.
        glEnable(GL_LIGHT0);
        glEnable(GL_LIGHTING);

        // Ambient and diffuse lighting track the current color.
        glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
        glEnable(GL_COLOR_MATERIAL);

        // Turn on normal vector normalization. You don't have to give unit
        // normal vector, and you can scale objects.
        glEnable(GL_NORMALIZE);

        // Set up the viewport.
        glViewport(0, 0, w(), h());

        // Set up the persepctive transformation.
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        fov_y = 360.0f / M_PI * atan(h() * tan(FOV_X * M_PI / 360.0) / w());
        gluPerspective(fov_y, w() / (float)h(), 1.0, 1000.0);

        // Do some light stuff. Diffuse color, and zero specular color
        // turns off specular lighting.
        color[0] = 1.0f; color[1] = 1.0f; color[2] = 1.0f; color[3] = 1.0f;
        glLightfv(GL_LIGHT0, GL_DIFFUSE, color);
        color[0] = 0.0f; color[1] = 0.0f; color[2] = 0.0f; color[3] = 1.0f;
        glLightfv(GL_LIGHT0, GL_SPECULAR, color);

        // Initialize all the objects.
        ground.Initialize();
        //horizon.Initialize();
        traintrack.Initialize();
        teacups.Initialize();
        carousel.Initialize();
        springTree.Initialize();
        summerTree.Initialize();
        fallTree.Initialize();
        winterTree.Initialize();
        globe.Initialize();
        hill.Initialize();
    }

    // Stuff out here relies on a coordinate system or must be done on every
    // frame.

    // Clear the screen. Color and depth.
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    // Camera
    switch (camera)
    {
        case FREE_CAM:
            // Set up the viewing transformation. The viewer is at a distance
            // dist from (x_at, y_ay, 2.0) in the direction (theta, phi) defined
            // by two angles. They are looking at (x_at, y_ay, 2.0) and z is up.
            eye[0] = x_at + dist * cos(theta * M_PI / 180.0) * cos(phi * M_PI / 180.0);
            eye[1] = y_at + dist * sin(theta * M_PI / 180.0) * cos(phi * M_PI / 180.0);
            eye[2] = 2.0 + dist * sin(phi * M_PI / 180.0);
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            gluLookAt(eye[0], eye[1], eye[2], x_at, y_at, 2.0, 0.0, 0.0, 1.0);
            break;
        case TRAIN_CAM:
            // Set up the viewing transformation. The viewer is sitting in the train.
            float x, y, z;
            train_pos[2] += 2.0f;   // move up in the seat

            // uncomment if you want to move backwards or 
            // forwards in the seat position
            //train_pos[0] -= 1.0f * train_dir[0];
            //train_pos[1] -= 1.0f * train_dir[1];
            //train_pos[2] -= 1.0f * train_dir[2];

            // setup the referece point along the -z direction
            x = train_pos[0] + train_dir[0];
            y = train_pos[1] + train_dir[1];
            z = train_pos[2] + train_dir[2];
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            gluLookAt(
                train_pos[0], train_pos[1], train_pos[2]    // eye
                , x, y, z                                   // reference point
                , 0.0, 0.0, 1.0                             // up direction
            );
            break;
    }

    // Position the light source. This has to happen after the viewing
    // transformation is set up, so that the light stays fixed in world
    // space. This is a directional light - note the 0 in the w component.
    dir[0] = 1.0; dir[1] = 1.0; dir[2] = 1.0; dir[3] = 0.0;
    glLightfv(GL_LIGHT0, GL_POSITION, dir);

    // Draw stuff. Everything.
    ground.Draw();
	//horizon.Draw();
    traintrack.Draw();

    glPushMatrix();
    glTranslatef(40.0f, -40.0f, 0.0f);
    hill.Draw();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 10.0f);
    globe.Draw();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(23.0f, 23.0f, 0.0f);
	teacups.Draw();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-13.0f, -33.0f, 0.0f);
    carousel.Draw();
    glPopMatrix();

    // Spring Forest
    for (unsigned int i = 0; i < springForest.size(); i += 3)
    {
        glPushMatrix();
        glTranslatef(springForest.at(i), springForest.at(i+1), springForest.at(i+2));
        springTree.Draw();
        glPopMatrix();
    }

    // Summer Forest
    for (unsigned int i = 0; i < summerForest.size(); i += 3)
    {
        glPushMatrix();
        glTranslatef(summerForest.at(i), summerForest.at(i+1), summerForest.at(i+2));
        summerTree.Draw();
        glPopMatrix();
    }

    // Fall Forest
    for (unsigned int i = 0; i < fallForest.size(); i += 3)
    {
        glPushMatrix();
        glTranslatef(fallForest.at(i), fallForest.at(i+1), fallForest.at(i+2));
        fallTree.Draw();
        glPopMatrix();
    }

    // Winter Forest
    for (unsigned int i = 0; i < winterForest.size(); i += 3)
    {
        glPushMatrix();
        glTranslatef(winterForest.at(i), winterForest.at(i+1), winterForest.at(i+2));
        winterTree.Draw();
        glPopMatrix();
    }
}


void
WorldWindow::Drag(float dt)
{
    int	    dx = x_down - x_last;
    int     dy = y_down - y_last;

    switch ( button )
    {
      case FL_LEFT_MOUSE:
	// Left button changes the direction the viewer is looking from.
	theta = theta_down + 360.0f * dx / (float)w();
	while ( theta >= 360.0f )
	    theta -= 360.0f;
	while ( theta < 0.0f )
	    theta += 360.0f;
	phi = phi_down + 90.0f * dy / (float)h();
	if ( phi > 89.0f )
	    phi = 89.0f;
	if ( phi < -5.0f )
	    phi = -5.0f;
	break;
      case FL_MIDDLE_MOUSE:
	// Middle button moves the viewer in or out.
	dist = dist_down - ( 0.5f * dist_down * dy / (float)h() );
	if ( dist < 1.0f )
	    dist = 1.0f;
	break;
      case FL_RIGHT_MOUSE: {
	// Right mouse button moves the look-at point around, so the world
	// appears to move under the viewer.
	float	x_axis[2];
	float	y_axis[2];

	x_axis[0] = -(float)sin(theta * M_PI / 180.0);
	x_axis[1] = (float)cos(theta * M_PI / 180.0);
	y_axis[0] = x_axis[1];
	y_axis[1] = -x_axis[0];

	x_at = x_at_down + 100.0f * ( x_axis[0] * dx / (float)w()
				    + y_axis[0] * dy / (float)h() );
	y_at = y_at_down + 100.0f * ( x_axis[1] * dx / (float)w()
				    + y_axis[1] * dy / (float)h() );
	} break;
      default:;
    }
}


bool
WorldWindow::Update(float dt)
{
    // Update the view. This gets called once per frame before doing the
    // drawing.

    if ( button != -1 ) // Only do anything if the mouse button is down.
	Drag(dt);

    // Animate the train, teacups, and carousel.
    traintrack.Update(dt, train_pos, train_dir);
    teacups.Update(dt);
    carousel.Update(dt);

    return true;
}


int
WorldWindow::handle(int event)
{
    // Event handling routine. Only looks at mouse events.
    // Stores a bunch of values when the mouse goes down and keeps track
    // of where the mouse is and what mouse button is down, if any.
    switch ( event )
    {
        case FL_PUSH:
            if (camera != FREE_CAM) return 1;
            button = Fl::event_button();
            x_last = x_down = Fl::event_x();
            y_last = y_down = Fl::event_y();
            phi_down = phi;
            theta_down = theta;
            dist_down = dist;
            x_at_down = x_at;
            y_at_down = y_at;
            return 1;
        case FL_DRAG:
            if (camera != FREE_CAM) return 1;
            x_last = Fl::event_x();
            y_last = Fl::event_y();
            return 1;
        case FL_RELEASE:
            if (camera != FREE_CAM) return 1;
            button = -1;
            return 1;
        case FL_KEYDOWN:
            switch (Fl::event_key())
            {
                case 'c':
                    camera = (camera == TRAIN_CAM) ? FREE_CAM : TRAIN_CAM;
                    break;
                case 's':
                    globe.Update();
                    break;
                default:
                    break;
            }
            return 1;
    }

    // Pass any other event types on the superclass.
    return Fl_Gl_Window::handle(event);
}


