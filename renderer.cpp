#include "renderer.h"

#include <QTextStream>
#include "draw.h"
#include "a2.h"

// constructor
Renderer::Renderer(QWidget *parent)
	: QOpenGLWidget(parent)
{
    g_world = new Point3D[4];
    g_world[0] = *new Point3D(0,0,0),
    g_world[1] = *new Point3D(0.5,0,0),
    g_world[2] = *new Point3D(0,0.5,0),
    g_world[3] = *new Point3D(0,0,0.5);

    points = new Point3D[8];
    set_perspective(30.0 / 180 * M_PI, 1, 0.1, 10000);
    reset_view();
}

// destructor
Renderer::~Renderer()
{
	// Nothing to do here right now.
}

// Force a rerender
void Renderer::invalidate()
{
	update();
}

void Renderer::set_perspective(double fov, double aspect,
                             double near, double far)
{
    // Fill me in!
    m_projection[0][0] = 1 / (aspect * atan(fov / 2));
    m_projection[1][1] = 1 / atan(fov / 2);
    m_projection[2][2] = -(far + near) / (far - near);
    m_projection[2][3] = -2 * (far * near) / (far - near);
    m_projection[3][2] = 1;
    m_projection[3][3] = 0;
}

void Renderer::reset_view()
{
    // Fill me in!
       m_view = *new Matrix4x4();
       m_view = translation(*new Vector3D(0,0,-40));
}

// called once by Qt GUI system, to allow initialization for OpenGL requirements
void Renderer::initializeGL()
{
    // You might not have anything in here, might have viewport & matrix setup...
}


// called by the Qt GUI system, to allow OpenGL drawing commands
void Renderer::paintGL()
{
	// Here is where your drawing code should go.

	draw_init(width(), height());

	/* A few of lines are drawn below to show how it's done. */

    set_colour(Colour(0.1, 0.1, 0.1));

	draw_line(Point2D(0.1*width(), 0.1*height()), 
		Point2D(0.9*width(), 0.9*height()));
	draw_line(Point2D(0.9*width(), 0.1*height()),
		Point2D(0.1*width(), 0.9*height()));

	draw_line(Point2D(0.1*width(), 0.1*height()),
		Point2D(0.2*width(), 0.1*height()));
	draw_line(Point2D(0.1*width(), 0.1*height()), 
		Point2D(0.1*width(), 0.2*height()));

    drawGnomon();
	draw_complete();
	    
}

// called by the Qt GUI system, to allow OpenGL to respond to widget resizing
void Renderer::resizeGL(int width, int height)
{
    // width() and height() are better methods to use
    Q_UNUSED(width); Q_UNUSED(height);

    // You might not have anything in here, might have viewport setup...
}

// override mouse press event
void Renderer::mousePressEvent(QMouseEvent * event)
{
    QTextStream cout(stdout);
    cout << "Stub: Button " << event->button() << " pressed.\n";
}


// override mouse release event
void Renderer::mouseReleaseEvent(QMouseEvent * event)
{
    QTextStream cout(stdout);
    cout << "Stub: Button " << event->button() << " released.\n";
}


// override mouse move event
void Renderer::mouseMoveEvent(QMouseEvent * event)
{
    QTextStream cout(stdout);
    cout << "Stub: Motion at " << event->x() << ", " << event->y() << ".\n";
}

void Renderer::drawGnomon()
{
    Point3D p = m_projection * m_view * m_model * g_world[0];
    Point2D a, b;

    a[0] = p[0] * width();
    a[1] = p[1] * height();

    p = m_projection * m_view * m_model * g_world[1];

    b[0] = p[0] * width();
    b[1] = p[1] * height();;

    draw_line(a, b);
}
