/*
 * Tree.h: Header file for a parameterized class that draws trees.
 */

#pragma once

#include <FL/gl.h>
#include <array>
#include <map>

enum Season {
    SPRING,
    SUMMER,
    FALL,
    WINTER
};

class Tree {
  private:
    GLubyte display_list;   // The display list that does all the work.
    //GLuint  texture_obj;    // The object for the trunk texture.
    bool    initialized;    // Whether or not we have been initialised.

    Season  season;
    GLfloat trunkHeight;
    GLfloat trunkRadius;
    GLfloat foliageHeight;
    GLfloat foliageRadius;

    // create a map of (Season, color array) pairs; this will set the foliage color based on season.
    static const std::map<Season, std::array<GLfloat, 3>> foliageColors;

  public:
    // Constructor
    Tree(Season s, GLfloat trunkHeight, GLfloat trunkRadius, GLfloat foliageHeight, GLfloat foliageRadius);

    // Destructor. Frees the display lists and texture object.
    ~Tree(void);

    // Initializer. Creates the display list.
    bool    Initialize(void);

    // Does the drawing.
    void    Draw(void);
};
