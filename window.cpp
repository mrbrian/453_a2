#include "window.h"

#include <QMenuBar>
#include <QApplication>
#include <QTextStream>
#include "a2.h"

int compare(double a, double b)
{
    if (fabs(a - b) < 0.0001)
        return 0;
    return 1;
}

int scaleMatrix()
{
    Matrix4x4 scale = scaling(*new Vector3D(-2, -5, 2));
    Vector3D result = *new Vector3D(1, -2, 3);
    result = scale * result;
    Vector3D expected = *new Vector3D(-2, 10, 6);

    for (int i = 0; i < 3; i++)
        if (compare(result[i], expected[i]))
            return i + 1;

    return 0;
}

int transMatrix()
{
    Matrix4x4 trans = translation(*new Vector3D(-5, 10, 3.5));
    Point3D result = *new Point3D(1, -2, 3);
    result = trans * result;
    Point3D expected = *new Point3D(-4, 8, 6.5);

    for (int i = 0; i < 3; i++)
        if (compare(result[i], expected[i]))
            return i + 1;

    return 0;
}

int rotateMatrix_X()
{
    Matrix4x4 rot = rotation(M_PI / 2, 'z');
    Vector3D result = *new Vector3D(1, -2, 3);
    result = rot * result;
    Vector3D expected = *new Vector3D(2, 1, 3);

    for (int i = 0; i < 3; i++)
        if (compare(result[i], expected[i]))
            return i + 1;

    return 0;
}

int rotateMatrix_Y()
{
    Matrix4x4 rot = rotation(M_PI / 2, 'y');
    Vector3D result = *new Vector3D(-1, 0, 0);
    result = rot * result;
    Vector3D expected = *new Vector3D(0, 0, -1);

    for (int i = 0; i < 3; i++)
        if (compare(result[i], expected[i]))
            return i + 1;

    return 0;
}

int rotateMatrix_Z()
{
    Matrix4x4 rot = rotation(M_PI / 2, 'z');
    Vector3D result = *new Vector3D(-1, 0, 2);
    result = rot * result;
    Vector3D expected = *new Vector3D(0, -1, 2);

    for (int i = 0; i < 3; i++)
        if (compare(result[i], expected[i]))
            return i + 1;

    return 0;
}

Matrix4x4 perspective(double fov, double aspect,
                             double n, double f)
{
    Matrix4x4 m_projection;
    // Fill me in!
    m_projection[0][0] = 1 / (aspect * tan(fov / 2));
    m_projection[1][1] = 1 / (aspect * tan(fov / 2));
    m_projection[2][2] = (f + n) / (f - n);
    m_projection[2][3] = -2 * (f * n) / (f - n);
    m_projection[3][2] = 1;
    m_projection[3][3] = 0;

    return m_projection;
}

int perspMatrix()
{
    Matrix4x4 p = perspective(90.0 / 180 * M_PI, 1, 1, 100);
    Point3D result = *new Point3D(10,0,100);
    Point3D result2 = *new Point3D(200,0,200);
    double dist1 = 100;
    double dist2 = 200;

    result = p * result;
    result2 = p * result2;

    result = *new Point3D(
        result[0] / result[2],
        result[1] / result[2],
        result[2] / result[2]
    );
    result2 = *new Point3D(
        result2[0] / result2[2],
        result2[1] / result2[2],
        result2[2] / result2[2]
    );
    return 0;
}

Point3D clipLine(Matrix4x4 persp, Point3D a, Point3D b, Point3D p, Vector3D n)
{
    Point3D pt_i;

    a = persp * a;
    b = persp * b;

    double prod_a = (a - p).dot(n);
    double prod_b = (b - p).dot(n);
    double t = -1;

    if (prod_a > 0 && prod_b > 0)
    {
        // draw
    }
    else if (prod_a < 0 && prod_b < 0)
    {
        // skip
    }
    else if (prod_a < 0 || prod_b < 0)
    {
        t = prod_a / (prod_a - prod_b);
        pt_i = a + t * (b - a);
    }
    return pt_i;
}

bool equals(const Point3D& a, const Point3D& b)
{
  if (!compare(a[0], b[0])
      && !compare(a[1], b[1])
      && !compare(a[2], b[2]))
      return true;
  else
      return false;
}

int clipTest()
{
    Matrix4x4 persp = perspective(90.0 / 180 * M_PI, 1, 1, 100);

    Point3D p1 = Point3D(2.5,0,5);
    Point3D p2 = Point3D(40,0,200);
    Point3D p = Point3D(0,0,10);
    Vector3D n = Vector3D(0,0,-1);

    Point3D i1 = clipLine(persp, p1, p2, p, n);
    Point3D i2 = clipLine(persp, p2, p1, p, n);

    if (equals(i1, i2))
        return 0;
    return 1;
}

Point3D viewClipTest(double v_l, double v_r, double v_t, double v_b, Matrix4x4 persp,
                     Point3D a, Point3D b, double n, double f)
{
    Point3D plane[] = { Point3D(0,0, n), Point3D(0,0, f),
                        Point3D(0,v_t,0), Point3D(0,v_b,0),
                        Point3D(v_r,0,0), Point3D(v_l,0,0)};
    Vector3D normal[] = {Vector3D(0,0,1), Vector3D(0,0,-1),
                         Vector3D(0,-1,0), Vector3D(0,1,0),
                         Vector3D(-1,0,0), Vector3D(1,0,0)};
    Point3D pt_i;

    float dist1 = a[2];
    float dist2 = b[2];

    a = persp * a;
    b = persp * b;

    a = Point3D(a[0] / dist1, a[1] / dist1, a[2] / dist1);
    b = Point3D(b[0] / dist2, b[1] / dist2, b[2] / dist2);

    for (int i = 0; i < 6; i++)
    {
        Point3D p = plane[i];
        Vector3D n = normal[i];
        double prod_a = (a - p).dot(n);
        double prod_b = (b - p).dot(n);
        double t = -1;

        if (prod_a > 0 && prod_b > 0)
        {
            // draw
        }
        else if (prod_a < 0 && prod_b < 0)
        {
            // skip
        }
        else if (prod_a < 0 || prod_b < 0)
        {
            t = prod_a / (prod_a - prod_b);
            pt_i = a + t * (b - a);

            if (prod_a < 0)
                a = pt_i;
            else
                b = pt_i;
        }
    }
    return pt_i;
}

int viewportTest()
{
    double f = 20;
    double n = 1;
    Matrix4x4 persp = perspective(90.0 / 180 * M_PI, 1, n, f);
    Point3D a = Point3D(-10,0,0.1);
    Point3D b = Point3D(20,0,21);
    Point3D pt_i = viewClipTest(0, 1, 0, 1, persp, a, b, n, f);

    if (pt_i[0] == 5)
        return 0;
    return 1;
}

float tests()
{
    double f = tan(M_PI_4);
//    return f;
    if (viewportTest())
        return 1;
    if (clipTest())
        return 1;
    if (perspMatrix())
        return 1;
    if (rotateMatrix_Z())
        return 1;
    if (rotateMatrix_Y())
        return 1;
    if (rotateMatrix_X())
        return 1;
    if (scaleMatrix())
        return 1;
    if (transMatrix())
        return 1;
    if (perspMatrix())
        return 1;
    return 0;
}

Window::Window(QWidget * parent) :
    QMainWindow(parent)
{
    QTextStream cout(stdout);
    cout << "test: " << QString::number(tests()) << ".\n";

    setWindowTitle("CPSC453: Boxes vs the World!");

    // Create the main drawing object
    renderer = new Renderer();
    renderer->setMinimumSize(300, 300);

    // Setup the application's widget collection
    QVBoxLayout * layout = new QVBoxLayout();

    // Add renderer
    layout->addWidget(renderer);

    QWidget * mainWidget = new QWidget();
    mainWidget->setLayout(layout);
    setCentralWidget(mainWidget);

	// Create the actions to be used by the menus
	createActions();

	// Create the menus
    // Setup the file menu
    mFileMenu = menuBar()->addMenu(tr("&File"));
    mFileMenu->addAction(mResetAction);  // add reset view
    mFileMenu->addAction(mQuitAction);  // add quitting

    // Setup the mode menu
    mModeMenu = menuBar()->addMenu(tr("&Mode"));
    mViewMenu = mModeMenu->addMenu(tr("&View"));
    mModelMenu = mModeMenu->addMenu(tr("&Model"));
    mViewMenu->addAction(mVRotateAction);
    mViewMenu->addAction(mVTransAction);
    mViewMenu->addAction(mVPerspAction);

    mModelMenu->addAction(mMRotateAction);
    mModelMenu->addAction(mMTransAction);
    mModelMenu->addAction(mMScaleAction);

    mModeMenu->addAction(mViewportAction);

    // Setup the quit button
    modeLabel = new QLabel(this);
    layout->addWidget(modeLabel);
    layout->setAlignment(modeLabel, Qt::AlignBottom);
    modeLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    modeLabel->setText("Model: Rotate");
    modeLabel->setAlignment(Qt::AlignBottom | Qt::AlignLeft);
}

// destructor
Window::~Window()
{
    delete renderer;
}

// helper function for creating actions
void Window::createActions()
{
    // Quits the application
    mQuitAction = new QAction(tr("&Quit"), this);
    mQuitAction->setShortcut(QKeySequence(Qt::Key_Q));
    mQuitAction->setStatusTip(tr("Quits the application"));
    connect(mQuitAction, SIGNAL(triggered()), qApp, SLOT(quit()));

    // Resets the view
    mResetAction = new QAction(tr("&Reset"), this);
    mResetAction->setShortcut(QKeySequence(Qt::Key_A));
    mResetAction->setStatusTip(tr("Resets the view"));
    connect(mResetAction, SIGNAL(triggered()), renderer, SLOT(reset_view()));

    mModeGroup = new QActionGroup(this);
    connect(mModeGroup, SIGNAL(triggered(QAction *)), this, SLOT(setMode(QAction *)));
    mModeGroup->setExclusive(true);

    mVRotateAction = new QAction(tr("&Rotate"), this);
    mVRotateAction->setShortcut(QKeySequence(Qt::Key_O));
    mVRotateAction->setStatusTip(tr("Rotates the view"));
    mModeGroup->addAction(mVRotateAction);

    mVTransAction = new QAction(tr("&Translate"), this);
    mVTransAction->setShortcut(QKeySequence(Qt::Key_N));
    mVTransAction->setStatusTip(tr("Translates the view"));
    mModeGroup->addAction(mVTransAction);

    mVPerspAction = new QAction(tr("&Perspective"), this);
    mVPerspAction->setShortcut(QKeySequence(Qt::Key_P));
    mVPerspAction->setStatusTip(tr("Adjust the perspective"));
    mModeGroup->addAction(mVPerspAction);

    mMRotateAction = new QAction(tr("&Rotate"), this);
    mMRotateAction->setShortcut(QKeySequence(Qt::Key_R));
    mMRotateAction->setStatusTip(tr("Rotates the model"));
    mModeGroup->addAction(mMRotateAction);

    mMTransAction = new QAction(tr("&Translate"), this);
    mMTransAction->setShortcut(QKeySequence(Qt::Key_T));
    mMTransAction->setStatusTip(tr("Translates the model"));
    mModeGroup->addAction(mMTransAction);

    mMScaleAction = new QAction(tr("&Scale"), this);
    mMScaleAction->setShortcut(QKeySequence(Qt::Key_S));
    mMScaleAction->setStatusTip(tr("Scale the model "));
    mModeGroup->addAction(mMScaleAction);

    mVRotateAction->setCheckable(true);
    mVTransAction->setCheckable(true);
    mVPerspAction->setCheckable(true);

    mMRotateAction->setCheckable(true);
    mMTransAction->setCheckable(true);
    mMScaleAction->setCheckable(true);

    mMRotateAction->setChecked(true);

    mViewportAction = new QAction(tr("&Viewport"), this);
    mViewportAction->setShortcut(QKeySequence(Qt::Key_V));
    mViewportAction->setStatusTip(tr("Adjust the viewport"));
    mModeGroup->addAction(mViewportAction);
}

void Window::setMode(QAction * action)
{
    if (action == mVRotateAction)
    {
        renderer->setMode(Renderer::VIEW_R);
        modeLabel->setText("View: Rotate");
    }
    else if (action == mVTransAction)
    {
        renderer->setMode(Renderer::VIEW_T);
        modeLabel->setText("View: Translate");
    }
    else if (action == mVPerspAction)
    {
        renderer->setMode(Renderer::VIEW_P);
        modeLabel->setText("View: Perspective");
    }
    else if (action == mMRotateAction)
    {
        renderer->setMode(Renderer::MODEL_R);
        modeLabel->setText("Model: Rotate");
    }
    else if (action == mMTransAction)
    {
        renderer->setMode(Renderer::MODEL_T);
        modeLabel->setText("Model: Translate");
    }
    else if (action == mMScaleAction)
    {
        renderer->setMode(Renderer::MODEL_S);
        modeLabel->setText("Model: Scale");
    }
    else
    {
        renderer->setMode(Renderer::VIEWPORT);
        modeLabel->setText("Viewport");
    }
}
