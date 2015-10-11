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

#include "draw.h"

#include <QOpenGLFunctions>
#include <QMatrix4x4>

void draw_line(const Point2D& p, const Point2D& q)
{
	QOpenGLFunctions glFuncs(QOpenGLContext::currentContext());
    glFuncs.initializeOpenGLFunctions(); // Qt support for inline GL function calls

	glVertex2d(p[0], p[1]);
	glVertex2d(q[0], q[1]);
}

void set_colour(const Colour& col)
{
	QOpenGLFunctions glFuncs(QOpenGLContext::currentContext());
    glFuncs.initializeOpenGLFunctions(); // Qt support for inline GL function calls

	glColor3f((float)col.R(), (float)col.G(), (float)col.B());
}

void draw_init(int width, int height)
{
	QOpenGLFunctions glFuncs(QOpenGLContext::currentContext());
    glFuncs.initializeOpenGLFunctions(); // Qt support for inline GL function calls

    glClearColor(0.7, 0.7, 0.7, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
    QMatrix4x4 ortho; // replaces gluOrtho2D
    ortho.ortho(0.0f, (float)width, 0.0f, (float)height,-1.0f, 1.0f);
    glMultMatrixf(ortho.constData());
	glViewport(0, 0, width, height);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslated(0.0, (double)height, 0.0);
	glScaled(1.0, -1.0, 1.0); 

	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glLineWidth(1.0);

	glBegin(GL_LINES);
}

void draw_complete()
{
	QOpenGLFunctions glFuncs(QOpenGLContext::currentContext());
    glFuncs.initializeOpenGLFunctions(); // Qt support for inline GL function calls

	glEnd();
    glFlush();
}
