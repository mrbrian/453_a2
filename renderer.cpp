#include "renderer.h"

#include <QTextStream>
#include "draw.h"
#include "a2.h"

// constructor
Renderer::Renderer(QWidget *parent)
	: QOpenGLWidget(parent)
{
    g_world = new Point3D[4]{
        *new Point3D(0,0,0),
        *new Point3D(0.5,0,0),
        *new Point3D(0,0.5,0),
        *new Point3D(0,0,0.5)
    };

    g_box = new Point3D[9]{
        *new Point3D(-1,1,-1),
        *new Point3D(-1,1,1),
        *new Point3D(1,1,1),
        *new Point3D(1,1,-1),
        *new Point3D(-1,1,-1),

        *new Point3D(-1,-1,-1),
        *new Point3D(-1,-1,1),
        *new Point3D(1,-1,1),
        *new Point3D(-1,-1,1)
    };

    float aspect = (float)width() / height();
    set_perspective(30.0 / 180 * M_PI, aspect, 0.1, 100);
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
                             double n, double f)
{
    // Fill me in!

    m_projection[0][0] = 1 / (aspect * tan(fov / 2));
    m_projection[1][1] = 1 / (aspect * tan(fov / 2));
    m_projection[2][2] = (f + n) / (f - n);
    m_projection[2][3] = -2 * (f * n) / (f - n);
    m_projection[3][2] = 1;
    m_projection[3][3] = 0;
}

void Renderer::reset_view()
{
    // Fill me in!
       m_view = *new Matrix4x4();
    //   m_view = rotation(45.0/180*M_PI, 'x') * rotation(M_PI_4, 'y');
       m_view = translation(*new Vector3D(0,0,10)) * m_view;
}

void Renderer::setupViewport()
{
    viewport_top    = height() * 0.05;
    viewport_left   = width() * 0.05;
    viewport_bottom = height() * 0.95;
    viewport_right  = width() * 0.95;
}

// called once by Qt GUI system, to allow initialization for OpenGL requirements
void Renderer::initializeGL()
{
    // You might not have anything in here, might have viewport & matrix setup...
    setupViewport();
}


// called by the Qt GUI system, to allow OpenGL drawing commands
void Renderer::paintGL()
{
	// Here is where your drawing code should go.

	draw_init(width(), height());

	/* A few of lines are drawn below to show how it's done. */

    set_colour(Colour(0.1, 0.1, 0.1));
/*
	draw_line(Point2D(0.1*width(), 0.1*height()), 
		Point2D(0.9*width(), 0.9*height()));
	draw_line(Point2D(0.9*width(), 0.1*height()),
		Point2D(0.1*width(), 0.9*height()));

    draw_line(Point2D(0.1*width(), 0.1*height()),
		Point2D(0.2*width(), 0.1*height()));
	draw_line(Point2D(0.1*width(), 0.1*height()), 
        Point2D(0.1*width(), 0.2*height()));*/

    std::vector<Line3D> demoLines = m_cube.getLines();
    Matrix4x4 model_matrix = m_cube.getTransform();

    for(std::vector<Line3D>::iterator it = demoLines.begin(); it != demoLines.end(); ++it)
    {
        Line3D line = *it;
        // Get the points and apply the model matrix
        Point3D p1 = model_matrix * line.getP1(), p2 = model_matrix * line.getP2();

        // Fill this in: Apply the view matrix
        p1 = m_view * p1;
        p2 = m_view * p2;
        float dist1 = p1[2];
        float dist2 = p2[2];

        // Fill this in: Do clipping here...

        // Apply the projection matrix
        p1 = m_projection * p1;
        p2 = m_projection * p2;

        p1[0] =  p1[0] * width() / dist1 + width() / 2;
        p1[1] = -p1[1] * height() / dist1 + height() / 2;

        p2[0] =  p2[0] * width() / dist2 + width() / 2;
        p2[1] = -p2[1] * height() / dist2 + height() / 2;
       // Fill this in: Do clipping here (maybe)

       draw_line(Point2D(p1[0], p1[1]), Point2D(p2[0], p2[1]));
    }

    drawViewport();
    drawGnomon();
    //drawBox();
	draw_complete();
	    
}

// called by the Qt GUI system, to allow OpenGL to respond to widget resizing
void Renderer::resizeGL(int width, int height)
{
    // width() and height() are better methods to use
    Q_UNUSED(width); Q_UNUSED(height);

    // You might not have anything in here, might have viewport setup...
    setupViewport();
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

void Renderer::drawViewport()
{
    Point2D topLeft = Point2D(viewport_left, viewport_top);
    Point2D botLeft = Point2D(viewport_left, viewport_bottom);
    Point2D topRight = Point2D(viewport_right, viewport_top);
    Point2D botRight = Point2D(viewport_right, viewport_bottom);

    draw_line(topLeft, botLeft);
    draw_line(botLeft, botRight);
    draw_line(botRight, topRight);
    draw_line(topLeft, topRight);
}

void Renderer::drawGnomon()
{
    Colour colours[] = {
        Colour(1,0,0),
        Colour(0,1,0),
        Colour(0,0,1)
    };

    Point3D p = m_projection * m_view * m_model * g_world[0];
    double dist = p[2];
    p = m_projection * m_view * m_model * g_world[0];
    Point2D a, b;

    a[0] = p[0] * width() / dist + width() / 2;
    a[1] = -p[1] * height() / dist + height() / 2;

    for (int i = 1; i < 4; i++)
    {
        set_colour(colours[i - 1]);
        p = m_view * m_model * g_world[i];
        dist = p[2];
        p = m_projection * p;

        b[0] = p[0] * width() /dist + width() / 2;
        b[1] = -p[1] * height() / dist + height() / 2;

        draw_line(a, b);
    }
}

void Renderer::drawBox()
{
    for (int i = 0; i < 4; i++)
    {
        Point2D a, b;
        Point3D p1 = m_view * m_model * g_box[i];
        double dist1 = p1[2];
        p1 = m_projection * p1;

        a[0] =  p1[0] * width() / dist1 + width() / 2;
        a[1] = -p1[1] * height() / dist1 + height() / 2;

        set_colour(Colour(0.1, 0.1, 0.1));
        Point3D p2 = m_view * m_model * g_box[i + 1];
        double dist2 = p2[2];

        p2 = m_projection * p2;
        b[0] =  p2[0] * width() / dist2 + width() / 2;
        b[1] = -p2[1] * height() / dist2 + height() / 2;

        draw_line(a, b);
    }
}
