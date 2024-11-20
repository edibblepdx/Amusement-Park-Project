/*
 * Tree.cpp: A class for drawing parameterized trees.
 */


#include <stdio.h>
#include <GL/glu.h>
#include "Tree.h"
#include "libtarga.h"

// create a map of (Season, color array) pairs; this will set the foliage color based on season.
const std::map<Season, std::array<GLfloat, 3>> Tree::foliageColors = {
        {SPRING, {0.6f, 0.9f, 0.6f}},   // light green
        {SUMMER, {0.6f, 0.7f, 0.6f}},   // dark green
        {FALL,   {0.8f, 0.4f, 0.0f}},   // orange
        {WINTER, {0.6f, 0.6f, 0.6f}}    // snowy
};

// Constructor
Tree::Tree(Season s, GLfloat trunkHeight, GLfloat trunkRadius, GLfloat foliageHeight, GLfloat foliageRadius)
    : display_list { 0 }
    //, texture_obj{0}
    , initialized{ false }
    , season{ s }
    , trunkHeight{ trunkHeight }
    , trunkRadius{ trunkRadius }
    , foliageHeight{ foliageHeight }
    , foliageRadius{ foliageRadius } 
{}

// Destructor
Tree::~Tree(void)
{
    if ( initialized )
    {
		glDeleteLists(display_list, 1);
		//glDeleteTextures(1, &texture_obj);
    }
}


// Initializer. Returns false if something went wrong
bool
Tree::Initialize(void)
{
    if (initialized) return true;   // already initialized

    // Create the display list.
    display_list = glGenLists(1);
    glNewList(display_list, GL_COMPILE);

    // Create a quadratic object
    GLUquadric *quad = gluNewQuadric();
    gluQuadricNormals(quad, GLU_SMOOTH);

    // Draw the trunk as a cylinder
    glColor3f(0.2f, 0.15f, 0.1f);   // brown
    gluCylinder(quad, trunkRadius, trunkRadius, trunkHeight, 16, 4);

    // Draw the foliage as a cone
    glColor3fv(foliageColors.at(season).data());    // foliage color based on season
    glTranslatef(0.0f, 0.0f, trunkHeight);          // place foliage on top of the trunk
    gluCylinder(quad, foliageRadius, 0.0f, foliageHeight, 16, 4);

    // Draw the bottom of the foliage as a disk
    gluQuadricOrientation(quad, GLU_INSIDE);   // have normals face -z
    gluDisk(quad, 0.0f, foliageRadius, 16, 1);

    // Destroy the quadratics object
    gluDeleteQuadric(quad);

    glEndList();

    initialized = true;
    return true;
}


// Draw just calls the display list we set up earlier.
void
Tree::Draw(void)
{
    glPushMatrix();
    glCallList(display_list);
    glPopMatrix();
}
