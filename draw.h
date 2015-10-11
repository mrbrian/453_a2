/****************************************************************************
 *
 * CPSC453 - Introduction to Computer Graphics
 * Assignment 2
 *
 * In Assignment 2, this code is responsible for drawing lines
 * in an OpenGL context.  We put it in a separate file to emphasize
 * that you shouldn't be using any OpenGL directly -- your drawing
 * function should only call functions declared here.
 *
 * Adapted from CS488 University of Waterloo Computer Graphics Lab / 2003-2005
 *
 ****************************************************************************/

#ifndef DRAW_H
#define DRAW_H

#include "algebra.h"

// Draw a line -- call draw_init first!
void draw_line(const Point2D& p, const Point2D& q);

// Set the current colour
void set_colour(const Colour& col);

// Call this before you begin drawing. Width and height are the width
// and height of the GL window.
void draw_init(int width, int height);

// Call this after all lines have been drawn for one frame
void draw_complete();

#endif // DRAW_H
