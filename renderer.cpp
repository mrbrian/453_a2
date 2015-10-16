#include "renderer.h"

#include <QTextStream>
#include "draw.h"
#include "a2.h"

#define DEF_VIEW_NEAR       5
#define DEF_VIEW_FAR        40
#define DEF_VIEW_FOV        30

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

    reset_view();
    update_projection();
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
    p_view = Vector3D(DEF_VIEW_NEAR, DEF_VIEW_FAR, DEF_VIEW_FOV);
    update_projection();

    m_cube.resetTransform();
    m_cubeGnomon = Matrix4x4();

    m_view = *new Matrix4x4();
    m_view = translation(t_view) * m_view;
    setupViewport();
    invalidate();
}

void Renderer::update_view()
{
    // Fill me in!
   // m_mapViewport[0][0] = (m_viewport[1][0] - m_viewport[0][0]) / 2;
   // m_mapViewport[1][1] = (m_viewport[1][1] - m_viewport[0][1]) / 2;
   // m_mapViewport[0][3] = (m_viewport[1][0] + m_viewport[0][0]) / 2;
   // m_mapViewport[1][3] = (m_viewport[1][1] + m_viewport[0][1]) / 2;
}

void Renderer::setupViewport()
{
    m_viewport[0][0] = width() * 0.05;
    m_viewport[0][1] = height() * 0.05;
    m_viewport[1][0] = width() * 0.95;
    m_viewport[1][1] = height() * 0.95;

    m_mapViewport = Matrix4x4();
    m_mapViewport[0][0] = width() / 2;
    m_mapViewport[1][1] = height() / 2;
    m_mapViewport = translation(Vector3D(width() / 2, height() / 2, 0)) * m_mapViewport;
}

void Renderer::update_projection()
{
    float aspect = (float)width() / height();
    set_perspective(p_view[2] / 180 * M_PI, aspect, p_view[0], p_view[1]);
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
    set_colour(Colour(0.1, 0.1, 0.1));
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
    if (editMode == VIEWPORT)
    {
        m_viewport[0] = Point2D(event->x(), event->y());
    }
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
    editValue(event->x() - p_mouseX);
    p_mouseX = event->x();

    if (editMode == VIEWPORT)
    {
        m_viewport[1] = Point2D(event->x(), event->y());
    }
}

void Renderer::drawViewport()
{
    // Draw viewport
    draw_line(m_viewport[0], Point2D(m_viewport[0][0], m_viewport[1][1]));
    draw_line(m_viewport[0], Point2D(m_viewport[1][0], m_viewport[0][1]));
    draw_line(m_viewport[1], Point2D(m_viewport[1][0], m_viewport[0][1]));
    draw_line(m_viewport[1], Point2D(m_viewport[0][0], m_viewport[1][1]));
}

void Renderer::drawGnomon(Matrix4x4 *model_matrix)
{
    Colour colours[] = {
        Colour(1,0,0),
        Colour(0,1,0),
        Colour(0,0,1)
    };

    Point3D a, b;
    Point3D p1 = (*model_matrix) * g_world[0];
    p1 = m_view * p1;
    double dist1 = p1[2];
    p1 = m_projection * p1;

    // homogenize
    a = scaling(Vector3D(1.0 / dist1, 1.0 / dist1, 1.0 / dist1)) * Point3D(p1[0], p1[1], 1);
    // transform to viewport
    a = m_mapViewport * a;

    for (int i = 1; i < 4; i++)
    {
        set_colour(colours[i - 1]);

        Point3D p2 = (*model_matrix) * g_world[i];
        p2 = m_view * p2;
        double dist2 = p2[2];
        p2 = m_projection * p2;
        // homogenize
        Matrix4x4 m_scale = scaling(Vector3D(1.0 / dist2, 1.0 / dist2, 1.0 / dist2));
        b = m_scale * Point3D(p2[0], p2[1], 1);
        // transform to viewport
        b = m_mapViewport * b;

        draw_line(Point2D(a[0], a[1]), Point2D(b[0], b[1]));
    }
}

void Renderer::drawBox()
{
    double view_l = ((m_viewport[0][0] * 2) - width()) / width();
    double view_r = ((m_viewport[1][0] * 2) - width()) / width();
    double view_t = ((m_viewport[0][1] * 2) - height()) / height();
    double view_b = ((m_viewport[1][1] * 2) - height()) / height();

    std::vector<Line3D> demoLines = m_cube.getLines();
    Matrix4x4 model_matrix = m_cube.getTransform();

    Point3D plane[] = {Point3D(0,0,p_view[0]), Point3D(0,0,p_view[1]),
                       Point3D(0,view_t,0), Point3D(0,view_b,0),
                       Point3D(view_r,0,0), Point3D(view_l,0,0)};

    Vector3D normal[] = {Vector3D(0,0,1), Vector3D(0,0,-1),
                         Vector3D(0,1,0), Vector3D(0,-1,0),
                         Vector3D(-1,0,0), Vector3D(1,0,0)};

    for(std::vector<Line3D>::iterator it = demoLines.begin(); it != demoLines.end(); ++it)
    {
        Line3D line = *it;
        // Get the points and apply the model matrix
        Point3D a = model_matrix * line.getP1(), b = model_matrix * line.getP2();

        // Fill this in: Apply the view matrix
        a = m_view * a;
        b = m_view * b;

        // Fill this in: Do clipping here...
        bool skipLine = false;
        Point3D pt_i;

        // clip near and far planes
        for (int i = 0; i < 2; i++)
        {
            // check if outside
            {
                Vector3D n = normal[i];
                Point3D p = plane[i];
                double t = (a - p).dot(n) / (a - b).dot(n);

                float dotProd_a = (a - p).dot(n);
                float dotProd_b = (b - p).dot(n);

                if (dotProd_a > 0 && dotProd_b > 0)         // both inside, line is fine
                {
                    continue;
                }
                else if (dotProd_a < 0 && dotProd_b < 0)    // both outside, dont draw this line
                {
                    skipLine = true;
                    break;
                }
                else if (dotProd_a < 0 || dotProd_b < 0)    // only one is outside
                {
                    t = dotProd_a / (dotProd_a - dotProd_b);
                    pt_i = a + t * (b - a);

                    if (dotProd_a < 0)  // replace with intersection point
                    {
                        a = pt_i;
                    }
                    else
                    {
                        b = pt_i;
                    }
                }
            }
        }
        if (skipLine)
            continue;

        double dist1 = a[2], dist2 = b[2];
        // Apply the projection matrix
        a = m_projection * a;
        b = m_projection * b;

        // homogenization
        a = Point3D(a[0] / dist1, a[1] / dist1, a[2] / dist1);
        b = Point3D(b[0] / dist2, b[1] / dist2, b[2] / dist2);

        for (int i = 2; i < 6; i++)
        {
            // check if outside
            {
                Vector3D n = normal[i];
                Point3D p = plane[i];
                double t = (a - p).dot(n) / (a - b).dot(n);

                float dotProd_a = (a - p).dot(n);
                float dotProd_b = (b - p).dot(n);

                if (dotProd_a > 0 && dotProd_b > 0)         // both inside, line is fine
                {
                    continue;
                }
                else if (dotProd_a < 0 && dotProd_b < 0)    // both outside, dont draw this line
                {
                    skipLine = true;
                    break;
                }
                else if (dotProd_a < 0 || dotProd_b < 0)    // only one is outside
                {
                    t = dotProd_a / (dotProd_a - dotProd_b);
                    pt_i = a + t * (b - a);

                    if (dotProd_a < 0)  // replace with intersection point
                    {
                        a = pt_i;
                    }
                    if (dotProd_b < 0)
                    {
                        b = pt_i;
                    }
                }
            }
        }
        if (skipLine)
            continue;

        // map to viewport
        a = m_mapViewport * a;
        b = m_mapViewport * b;

       // Fill this in: Do clipping here (maybe)

       draw_line(Point2D(a[0], a[1]), Point2D(b[0], b[1]));
    }
}

void Renderer::editValue(int value)
{
    Vector3D temp;
    float delta = (float)value / 100;

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
        p_view = p_view + temp;
        update_projection();
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
        update_view();
        break;
    }
    m_cube.appendTransform(modelTrans);
    m_cubeGnomon = m_cubeGnomon * gnomonTrans;

    invalidate();
}

void Renderer::setMode(EditMode mode)
{
    editMode = mode;
}

