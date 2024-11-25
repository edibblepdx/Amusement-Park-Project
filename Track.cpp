/*
 * Track.cpp: A class that draws the train and its track.
 *
 * (c) 2001-2002: Stephen Chenney, University of Wisconsin at Madison.
 */


#include <stdio.h>
#include <iostream>
#include <FL/math.h>
#include <GL/glew.h>
#include <GL/glu.h>
#include <cmath>
#include "Track.h"
#include "GenericException.h"
#include "objloader.h"


// The control points for the track spline.
const int   Track::TRACK_NUM_CONTROLS = 4;
const float Track::TRACK_CONTROLS[TRACK_NUM_CONTROLS][3] =
		{ { -20.0, -20.0, -18.0 }, { 20.0, -20.0, 40.0 },
		  { 20.0, 20.0, -18.0 }, { -20.0, 20.0, 40.0 } };

// The carriage energy and mass
const float Track::TRAIN_ENERGY = 250.0f;


// Normalize a 3d vector.
static void
Normalize_3(float v[3])
{
    double  l = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);

    if ( l == 0.0 )
	return;

    v[0] /= (float)l;
    v[1] /= (float)l;
    v[2] /= (float)l;
}


// Destructor
Track::~Track(void)
{
    if ( initialized )
    {
        glDeleteLists(track_list, 1);
        glDeleteLists(train_list, 1);
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &vertexbuffer);
        glDeleteBuffers(1, &uvbuffer);
        glDeleteBuffers(1, &normalbuffer);
    }
}


// Initializer. Would return false if anything could go wrong.
bool
Track::Initialize(void)
{
    CubicBspline    refined(3, true);
    int		    n_refined;
    float	    p[3];
    int		    i;

    // Create the track spline.
    track = new CubicBspline(3, true);
    for (i = 0; i < TRACK_NUM_CONTROLS; i++)
        track->Append_Control(TRACK_CONTROLS[i]);

    // Refine it down to a fixed tolerance. This means that any point on
    // the track that is drawn will be less than 0.1 units from its true
    // location. In fact, it's even closer than that.
    track->Refine_Tolerance(refined, 0.1f);
    n_refined = refined.N();

    // Create the display list for the track - just a set of line segments
    // We just use curve evaluated at integer paramer values, because the
    // subdivision has made sure that these are good enough.
    /*
    track_list = glGenLists(1);
    glNewList(track_list, GL_COMPILE);
    glColor3f(1.0f, 1.0, 1.0f);
    glBegin(GL_LINE_STRIP);
        for ( i = 0 ; i <= n_refined ; i++ )
        {
        refined.Evaluate_Point((float)i, p);
        glVertex3fv(p);
        }
    glEnd();
    glEndList();
    */

    // Create a quadratic object
    GLUquadric* quad = gluNewQuadric();
    gluQuadricNormals(quad, GLU_SMOOTH);
    float       j{};
    float       step{ 0.25 };
    float       tangent[3];
    float       axis[3];
    double      angle{ 0.0 };
    GLdouble    radius{ 0.15 };
    GLdouble    height{ 0.4 };
    GLint       slices{ 8 };
    GLint       stacks{ 2 };

    // Create the display list for the track as a swept object
    track_list = glGenLists(1);
    glNewList(track_list, GL_COMPILE);
	glColor3f(0.6f, 0.6f, 0.6f);    // gray
	for ( j = 0.0 ; j <= n_refined ; j+=step ) // loop over n_refined control points
	{
        // Find the point along the BSpline.
		refined.Evaluate_Point((float)j, p);
        glPushMatrix();

        glTranslatef(p[0], p[1], p[2]);

        // Find the tangent to the BSpline.
		refined.Evaluate_Derivative((float)j, tangent);
		Normalize_3(tangent);

        // We want to orient +z in the direction of the tangent line.
        // Find (0, 0, 1) x (t1, t2, t3). This will be our axis of rotation.
        axis[0] = -tangent[1];
        axis[1] = tangent[0];
        axis[2] = 0.0f;
        Normalize_3(axis);

        // From the dot product we find the angle between +z and the tangent line to the BSpline.
        // Rotate around 'axis', 'angle' degrees.
        angle = acos(tangent[2]) * 180.0 / M_PI;
        glRotatef((float)angle, axis[0], axis[1], axis[2]);

        // draw the Cylinder
        //gluCylinder(quad, radius, radius, height, slices, stacks);

		glPopMatrix();
        
        // draw inner track
        glPushMatrix();
            glTranslatef(0.95 * p[0], 0.95 * p[1], p[2]);
            glRotatef((float)angle, axis[0], axis[1], axis[2]);
            gluCylinder(quad, radius, radius, height, slices, stacks);
		glPopMatrix();

        // draw outer track
        glPushMatrix();
            glTranslatef(1.05 * p[0], 1.05 * p[1], p[2]);
            glRotatef((float)angle, axis[0], axis[1], axis[2]);
            gluCylinder(quad, radius, radius, height, slices, stacks);
		glPopMatrix();
       
        // draw cross beams
        /*
        if ( std::fmod(j, 1.0) == 0.0 )
        {
            // don't know how
        }
        */

        // add supports
        if ( std::fmod(j, 10.0) == 0.0 )
        {
			glPushMatrix();
			glTranslatef(p[0], p[1], 0.0);
			// Draw the Support Cylinder
			gluCylinder(quad, radius, radius, p[2], slices, stacks);
			glPopMatrix();
        }
	}
    glEndList();

    // Destroy the quadratics object
    gluDeleteQuadric(quad);

    // Load my train model
    if (!ObjLoader("train_car.obj", train_vertices, train_uvs, train_normals))
        throw new GenericException("Track::C - Failed to load track car");

    // create vertex array object
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // vertexbuffer
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, train_vertices.size() * sizeof(glm::vec3), train_vertices.data(), GL_STATIC_DRAW);

    /*
    // 1st attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0,                  // attribute
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
    );
    */
    
    // uvbuffer
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, train_uvs.size() * sizeof(glm::vec2), train_uvs.data(), GL_STATIC_DRAW);

    /*
    // 2nd attribute buffer : UVs
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1,                  // attribute
        2,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
    );
    */

    // normalbuffer
	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, train_normals.size() * sizeof(glm::vec3), train_normals.data(), GL_STATIC_DRAW);

    /*
    // 3rd attribute buffer : normals
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(
        2,                  // attribute
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
    );
    */

    // unbind VAO
    glBindVertexArray(0);
    
    initialized = true;

    return true;
}


// Draw
void
Track::Draw(void)
{
    float   posn[3];
    float   tangent[3];
    double  angle;

    if ( ! initialized )
	return;

    glPushMatrix();

    // Draw the track
    glCallList(track_list);

    for (int i = 0; i < 1; ++i)
    {
        glPushMatrix();
        
        //float train_pos = posn_on_track + 0.11 * i;
        //if ( train_pos > track->N() )
        //train_pos -= track->N();
        
        // Figure out where the train is
        track->Evaluate_Point(posn_on_track, posn);

        // Translate the train to the point
        // move it a little above the track
        glTranslatef(posn[0], posn[1], posn[2]+0.3);

        // ...and what it's orientation is
        track->Evaluate_Derivative(posn_on_track, tangent);
        Normalize_3(tangent);

        // Rotate it to point along the track, but stay horizontal
        angle = atan2(tangent[1], tangent[0]) * 180.0 / M_PI;
        glRotatef((float)angle, 0.0f, 0.0f, 1.0f);

        // Another rotation to get the tilt right.
        angle = asin(-tangent[2]) * 180.0 / M_PI;
        glRotatef((float)angle, 0.0f, 1.0f, 0.0f);

        // Because the car was sideways
        glRotatef((float)90, 0.0f, 0.0f, -1.0f);

        glColor3f(0.9f, 0.0f, 0.0f);

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
        glDrawArrays(GL_TRIANGLES, 0, train_vertices.size());
        
        //glBindVertexArray(VAO);
        //glDrawArrays(GL_TRIANGLES, 0, train_vertices.size());
        //glBindVertexArray(0);
        
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);

        glPopMatrix();
        glPopMatrix();
    }
}


void
Track::Update(float dt)
{
    float   point[3];
    float   deriv[3];
    double  length;
    double  parametric_speed;

    if ( ! initialized )
	return;

    // First we move the train along the track with its current speed.

    // Get the derivative at the current location on the track.
    track->Evaluate_Derivative(posn_on_track, deriv);

    // Get its length.
    length = sqrt(deriv[0]*deriv[0] + deriv[1]*deriv[1] + deriv[2]*deriv[2]);
    if ( length == 0.0 )
	return;

    // The parametric speed is the world train speed divided by the length
    // of the tangent vector.
    parametric_speed = speed / length;

    // Now just evaluate dist = speed * time, for the parameter.
    posn_on_track += (float)(parametric_speed * dt);

    // If we've just gone around the track, reset back to the start.
    if ( posn_on_track > track->N() )
	posn_on_track -= track->N();

    // As the second step, we use conservation of energy to set the speed
    // for the next time.
    // The total energy = z * gravity + 1/2 speed * speed, assuming unit mass
    track->Evaluate_Point(posn_on_track, point);
    if ( TRAIN_ENERGY - 9.81 * point[2] < 0.0 )
	speed = 0.0;
    else
	speed = (float)sqrt(2.0 * ( TRAIN_ENERGY - 9.81 * point[2] ));
}


