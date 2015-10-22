#include "renderer.h"

#include <QTextStream>
#include "draw.h"
#include "a2.h"

#define DEF_VIEW_NEAR       5
#define DEF_VIEW_FAR        15
#define DEF_VIEW_FOV        30
#define DEF_VIEW_DIST       10

// constructor
Renderer::Renderer(QWidget *parent)
    : QOpenGLWidget(parent)
{
    editMode = MODEL_R;

    g_gnomon = new Point3D[4]{     // the 4 points of the gnomon
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
    m_projection[0][0] = 1 / (aspect * tan(fov / 2));
    m_projection[1][1] = 1 / (aspect * tan(fov / 2));
    m_projection[2][2] = (f + n) / (f - n);
    m_projection[2][3] = -2 * (f * n) / (f - n);
    m_projection[3][2] = 1;
    m_projection[3][3] = 0;
}

void Renderer::reset_view()
{
    Vector3D t_view = Vector3D(0, 0, DEF_VIEW_DIST);
    p_view = Vector3D(DEF_VIEW_FOV, DEF_VIEW_NEAR, DEF_VIEW_FAR);       // use default values
    update_projection();

    m_cube.resetTransform();
    m_cubeGnomon = Matrix4x4();

    m_view = *new Matrix4x4();
    m_view = translation(t_view) * m_view;
    setupViewport();
    invalidate();           // trigger window update
}

void Renderer::setupViewport()
{
    m_viewport[0][0] = width() * 0.05;
    m_viewport[0][1] = height() * 0.05;
    m_viewport[1][0] = width() * 0.95;
    m_viewport[1][1] = height() * 0.95;

    m_screenCoords = Matrix4x4();           // make transform for converting NDC space to screenspace
    m_screenCoords[0][0] = width() / 2;
    m_screenCoords[1][1] = -height() / 2;
    m_screenCoords = translation(Vector3D(width() / 2, height() / 2, 0)) * m_screenCoords;
}

void Renderer::update_projection()
{
    float aspect = (float)width() / height();
    set_perspective(p_view[0] / 180 * M_PI, aspect, p_view[1], p_view[2]);
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
    drawGnomon(&m);         // m is the world transform  (identity since never changes)

    set_colour(Colour(0.1, 0.1, 0.1));
    drawCube();                         // draw cube
    drawGnomon(&m_cubeGnomon);          // draw gnomon using the cube's transform

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
    // define new viewport coords
    if (editMode == VIEWPORT)
    {
        m_viewport[0] = Point2D(event->x(), event->y());
        m_viewport[1] = Point2D(event->x(), event->y());
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
    modifyValue(event->x() - p_mouseX);

    if (editMode == VIEWPORT)
    {
        int c_x = event->x();
        int c_y = event->y();

        if (c_x > m_viewport[0][0])
        {
            m_viewport[1][0] = c_x;
        }
        else
        {
            m_viewport[0][0] = c_x;
        }

        if (c_y > m_viewport[0][1])
        {
            m_viewport[1][1] = c_y;
        }
        else
        {
            m_viewport[0][1] = c_y;
        }
    }
    p_mouseX = event->x();
}

void Renderer::drawViewport()
{
    // Drawing viewport bounds
    draw_line(m_viewport[0], Point2D(m_viewport[0][0], m_viewport[1][1]));
    draw_line(m_viewport[0], Point2D(m_viewport[1][0], m_viewport[0][1]));
    draw_line(m_viewport[1], Point2D(m_viewport[1][0], m_viewport[0][1]));
    draw_line(m_viewport[1], Point2D(m_viewport[0][0], m_viewport[1][1]));
}

void Renderer::drawGnomon(Matrix4x4 *model_matrix)
{
    Colour colours[] = {    // 3 colours of the gnomon lines
        Colour(1,0,0),
        Colour(0,1,0),
        Colour(0,0,1)
    };

    Point3D p1 = (*model_matrix) * g_gnomon[0];

    for (int i = 1; i < 4; i++)         // draw 3 lines between the 4 points
    {
        set_colour(colours[i - 1]);

        Point3D p2 = (*model_matrix) * g_gnomon[i];

        draw_line_3d(p1, p2);
    }
}

void Renderer::draw_line_3d(Point3D a, Point3D b)
{
    Matrix4x4 invTransform = m_screenCoords.invert();   // get transform to undo the screen-space transform

    Point3D ndc_view_1 = Point3D(m_viewport[0][0], m_viewport[0][1], 1);
    ndc_view_1 = invTransform * ndc_view_1;

    Point3D ndc_view_2 = Point3D(m_viewport[1][0], m_viewport[1][1], 1);
    ndc_view_2 = invTransform * ndc_view_2;

    double view_l = ndc_view_1[0];      // viewport boundaries in NDC
    double view_r = ndc_view_2[0];
    double view_t = ndc_view_1[1];
    double view_b = ndc_view_2[1];

    Point3D plane[] = {Point3D(0,0,p_view[1]), Point3D(0,0,p_view[2]),  // points on the bounding planes
                       Point3D(0,view_t,0), Point3D(0,view_b,0),
                       Point3D(view_r,0,0), Point3D(view_l,0,0)};

    Vector3D normal[] = {Vector3D(0,0,1), Vector3D(0,0,-1),             // normals of the bounding planes
                         Vector3D(0,-1,0), Vector3D(0,1,0),
                         Vector3D(-1,0,0), Vector3D(1,0,0)};

    // Apply the view matrix
    a = m_view * a;
    b = m_view * b;

    // Do clipping here...
    bool skipLine = false;
    Point3D pt_i;

    // clip near and far planes
    for (int i = 0; i < 2; i++)
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
    if (skipLine)
        return;

    // Apply the projection matrix to 4D points
    Point4D a_4 = Point4D(a);
    Point4D b_4 = Point4D(b);

    a_4 = m_projection * a_4;
    b_4 = m_projection * b_4;

    // homogenization & store back into 3D points
    a = Point3D(a_4[0] / a_4[3], a_4[1] / a_4[3], a_4[2] / a_4[3]);
    b = Point3D(b_4[0] / b_4[3], b_4[1] / b_4[3], b_4[2] / b_4[3]);

    // clip to the top/bot/left/right planes
    for (int i = 2; i < 6; i++)
    {
        // check if outside
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

            if (dotProd_a < 0)          // replace with intersection point
            {
                a = pt_i;
            }
            if (dotProd_b < 0)
            {
                b = pt_i;
            }
        }
    }
    if (skipLine)
        return;

    // map to viewport
    a = m_screenCoords * a;
    b = m_screenCoords * b;

   draw_line(Point2D(a[0], a[1]), Point2D(b[0], b[1]));
}

void Renderer::drawCube()
{
    std::vector<Line3D> demoLines = m_cube.getLines();
    Matrix4x4 model_matrix = m_cube.getTransform();

    for(std::vector<Line3D>::iterator it = demoLines.begin(); it != demoLines.end(); ++it)
    {
        Line3D line = *it;
        // Get the points and apply the model matrix
        Point3D a = model_matrix * line.getP1(), b = model_matrix * line.getP2();

       draw_line_3d(a, b);  // do clipping and projection, draw line
    }
}

void Renderer::modifyValue(int value)
{
    Vector3D delta;
    float d = (float)value / 100;

    if (mouseButtons & Qt::LeftButton)      // LB modifies along x-axis   (fov if viewport mode)
    {
        delta[0] = d;
    }
    if (mouseButtons & Qt::MiddleButton)    // MB modifies  along y-axis  (near-plane if viewport mode)
    {
        delta[1] = d;
    }
    if (mouseButtons & Qt::RightButton)     // RB modifies  along z-axis  (far-plane if viewport mode)
    {
        delta[2] = d;
    }

    Matrix4x4 modelTrans;       // the transforms we will compose onto model / gnomon
    Matrix4x4 gnomonTrans;

    switch(editMode)
    {
    case VIEW_R:
        m_view = rotation(delta[2], 'z')            // add the rotations to m_view
                * rotation(delta[1], 'y')
                * rotation(delta[0], 'x') * m_view;
        break;
    case VIEW_T:
        m_view = translation(delta) * m_view;       // add the translate to m_view
        break;
    case VIEW_P:
        p_view = p_view + delta;                    // change the parameters of the perspective transform (fov, near, far)
        update_projection();                        // update m_projection
        break;
    case MODEL_R:
        gnomonTrans = modelTrans = rotation(delta[2], 'z') * rotation(delta[1], 'y') * rotation(delta[0], 'x');
        break;
    case MODEL_S:
        modelTrans = scaling(Vector3D(1,1,1) + delta);
        break;
    case MODEL_T:
        gnomonTrans = modelTrans = translation(delta);
        break;
    }
    m_cube.appendTransform(modelTrans);             // add transform to cube transform
    m_cubeGnomon = m_cubeGnomon * gnomonTrans;      // and add to gnomon transform

    invalidate();                                   // trigger window update
}

void Renderer::setMode(EditMode mode)       // public set method for editmode
{
    editMode = mode;
}

