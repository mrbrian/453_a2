#include "renderer.h"

#include <QTextStream>
#include "draw.h"
#include "a2.h"

// constructor
Renderer::Renderer(QWidget *parent)
	: QOpenGLWidget(parent)
{
    editMode = MODEL_R;

    g_world = new Point3D[4]{
        *new Point3D(0,0,0),
        *new Point3D(0.5,0,0),
        *new Point3D(0,0.5,0),
        *new Point3D(0,0,0.5)
    };

    update_projection();
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
    Vector3D t_view = Vector3D(0, 0, 10);

    m_cube.resetTransform();
    m_cubeGnomon = Matrix4x4();

    m_view = *new Matrix4x4();
    //m_view = rotation(45.0/180*M_PI, 'x') * rotation(M_PI_4, 'y');
    m_view = translation(t_view) * m_view;
    invalidate();
}

void Renderer::update_view()
{
    // Fill me in!
/*       m_view = *new Matrix4x4();
       m_view = rotation(r_view[2], 'z')
               * rotation(r_view[1], 'y')
               * rotation(r_view[0], 'x')
               * translation(t_view) * m_view;*/
}

void Renderer::setupViewport()
{
    viewport_top    = height() * 0.05;
    viewport_left   = width() * 0.05;
    viewport_bottom = height() * 0.95;
    viewport_right  = width() * 0.95;
}

void Renderer::update_projection()
{
    float aspect = (float)width() / height();
    set_perspective(30.0 / 180 * M_PI, aspect, 0.1, 100);
}

// called once by Qt GUI system, to allow initialization for OpenGL requirements
void Renderer::initializeGL()
{
    // You might not have anything in here, might have viewport & matrix setup...
    setupViewport();
    update_projection();
}

// called by the Qt GUI system, to allow OpenGL drawing commands
void Renderer::paintGL()
{
	// Here is where your drawing code should go.

	draw_init(width(), height());

	/* A few of lines are drawn below to show how it's done. */

    set_colour(Colour(0.1, 0.1, 0.1));

    drawViewport();

    Matrix4x4 m;
    drawGnomon(&m);
    drawBox();
    drawGnomon(&m_cubeGnomon);
	draw_complete();
	    
}

// called by the Qt GUI system, to allow OpenGL to respond to widget resizing
void Renderer::resizeGL(int width, int height)
{
    // width() and height() are better methods to use
    Q_UNUSED(width); Q_UNUSED(height);

    // You might not have anything in here, might have viewport setup...
    setupViewport();
    update_projection();
}

// override mouse press event
void Renderer::mousePressEvent(QMouseEvent * event)
{
    QTextStream cout(stdout);
    cout << "Stub: Button " << event->button() << " pressed.\n";    
    p_mouseX = event->x();

    // save buttons
    mouseButtons = event->buttons();
}


// override mouse release event
void Renderer::mouseReleaseEvent(QMouseEvent * event)
{
    QTextStream cout(stdout);
    cout << "Stub: Button " << event->button() << " released.\n";

    // save buttons
    mouseButtons = event->buttons();
}

// override mouse move event
void Renderer::mouseMoveEvent(QMouseEvent * event)
{
    QTextStream cout(stdout);
    cout << "Stub: Motion at " << event->x() << ", " << event->y() << ".\n";    
    move(event->x() - p_mouseX);
    p_mouseX = event->x();
}

void Renderer::drawViewport()
{
    Point2D topL = Point2D(viewport_left, viewport_top);
    Point2D botL = Point2D(viewport_left, viewport_bottom);
    Point2D topR = Point2D(viewport_right, viewport_top);
    Point2D botR = Point2D(viewport_right, viewport_bottom);

    draw_line(topL, botL);
    draw_line(botL, botR);
    draw_line(botR, topR);
    draw_line(topL, topR);
}

void Renderer::drawGnomon(Matrix4x4 *model_matrix)
{
    Colour colours[] = {
        Colour(1,0,0),
        Colour(0,1,0),
        Colour(0,0,1)
    };

    Point3D p = m_view * (*model_matrix) * g_world[0];
    double dist = p[2];
    p = m_projection * p;
    Point2D a, b;

    a[0] = p[0] * width() / dist + width() / 2;
    a[1] = -p[1] * height() / dist + height() / 2;

    for (int i = 1; i < 4; i++)
    {
        set_colour(colours[i - 1]);
        p = m_view * *model_matrix * g_world[i];
        dist = p[2];
        p = m_projection * p;

        b[0] = p[0] * width() / dist + width() / 2;
        b[1] = -p[1] * height() / dist + height() / 2;

        b[0] = p[0] * width() / dist + width() / 2;
        b[1] = -p[1] * height() / dist + height() / 2;

        draw_line(a, b);
    }
}

void Renderer::drawBox()
{
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

	// so..  do clipping for each side.

	// check since NDC?	
	// then .. 

        // Apply the projection matrix
        p1 = m_projection * p1;
        p2 = m_projection * p2;

        // homogenization
        p1 =  Point3D(p1[0] / dist1, -p1[1] / dist1, 1);
        p2 =  Point3D(p2[0] / dist2, -p2[1] / dist2, 1);

        // map to viewport
        p1[0] =  p1[0] * width() + width() / 2;
        p1[1] = -p1[1] * height() + height() / 2;

        p2[0] =  p2[0] * width() + width() / 2;
        p2[1] = -p2[1] * height() + height() / 2;

       // Fill this in: Do clipping here (maybe)

       draw_line(Point2D(p1[0], p1[1]), Point2D(p2[0], p2[1]));
    }
}

void Renderer::move(int x)
{
    Vector3D temp;
    float delta = (float)x / 100;

    if (mouseButtons & Qt::LeftButton)      // LB rotates along x-axis
    {
        temp[0] = delta;
    }
    if (mouseButtons & Qt::MiddleButton)    // MB rotates along y-axis
    {
        temp[1] = delta;
    }
    if (mouseButtons & Qt::RightButton)     // RB rotates along z-axis
    {
        temp[2] = delta;
    }

    Matrix4x4 modelTrans;
    Matrix4x4 gnomonTrans;

    switch(editMode)
    {
    case VIEW_R:
        m_view = rotation(temp[2], 'z')
                * rotation(temp[1], 'y')
                * rotation(temp[0], 'x') * m_view;
        break;
    case VIEW_T:
        m_view = translation(temp) * m_view;
        break;
    case VIEW_P:
        m_view = *new Matrix4x4();
        break;
    case MODEL_R:
        gnomonTrans = modelTrans = rotation(temp[2], 'z') * rotation(temp[1], 'y') * rotation(temp[0], 'x');
        break;
    case MODEL_S:
        modelTrans = scaling(Vector3D(1,1,1) + temp);
        break;
    case MODEL_T:
        gnomonTrans = modelTrans = translation(temp);
        break;
    case VIEWPORT:
        break;
    }
    m_cube.appendTransform(modelTrans);
    m_cubeGnomon = gnomonTrans * m_cubeGnomon;

    invalidate();
}

void Renderer::setMode(EditMode mode)
{
    editMode = mode;
}

