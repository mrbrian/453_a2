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
    float d = 10;
    Point3D expected = *new Point3D(0,0,0);
    result = p * result;

    result = *new Point3D(
        result[0] / d,
        result[1] / d,
        result[2] / d
    );

    return 0;
}

float tests()
{
    double f = tan(M_PI_4);
//    return f;
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
	setCentralWidget(renderer);

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
    mViewMenu->addAction(mPerspAction);

    mModelMenu->addAction(mMRotateAction);
    mModelMenu->addAction(mMTransAction);
    mModelMenu->addAction(mScaleAction);
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
    connect(mResetAction, SIGNAL(triggered()), qApp, SLOT(quit()));

    mModeGroup = new QActionGroup(this);
    //connect(mModeGroup, SIGNAL(triggered(QAction *)), this, SLOT(setMode(QAction *)));
    mModeGroup->setExclusive(true);

    mVRotateAction = new QAction(tr("&Rotate"), this);
    mVRotateAction->setShortcut(QKeySequence(Qt::Key_O));
    mVRotateAction->setStatusTip(tr("Rotates the view"));
    mModeGroup->addAction(mVRotateAction);

    mVTransAction = new QAction(tr("&Translate"), this);
    mVTransAction->setShortcut(QKeySequence(Qt::Key_N));
    mVTransAction->setStatusTip(tr("Translates the view"));
    mModeGroup->addAction(mVTransAction);

    mPerspAction = new QAction(tr("&Perspective"), this);
    mPerspAction->setShortcut(QKeySequence(Qt::Key_P));
    mPerspAction->setStatusTip(tr("Adjust the perspective"));
    mModeGroup->addAction(mPerspAction);

    mMRotateAction = new QAction(tr("&Rotate"), this);
    mMRotateAction->setShortcut(QKeySequence(Qt::Key_R));
    mMRotateAction->setStatusTip(tr("Rotates the model"));
    mModeGroup->addAction(mMRotateAction);

    mMTransAction = new QAction(tr("&Translate"), this);
    mMTransAction->setShortcut(QKeySequence(Qt::Key_T));
    mMTransAction->setStatusTip(tr("Translates the model"));
    mModeGroup->addAction(mMTransAction);

    mScaleAction = new QAction(tr("&Scale"), this);
    mScaleAction->setShortcut(QKeySequence(Qt::Key_S));
    mScaleAction->setStatusTip(tr("Scale the model "));
    mModeGroup->addAction(mScaleAction);

    mVRotateAction->setCheckable(true);
    mVTransAction->setCheckable(true);
    mPerspAction->setCheckable(true);

    mMRotateAction->setCheckable(true);
    mMTransAction->setCheckable(true);
    mScaleAction->setCheckable(true);

    mViewportAction = new QAction(tr("&Viewport"), this);
    mViewportAction->setShortcut(QKeySequence(Qt::Key_S));
    mViewportAction->setStatusTip(tr("Adjust the perspective"));
    mModeGroup->addAction(mViewportAction);
}

void Window::setMode(QAction * action)
{/*
    if (action == mRotateAction)
        renderer->setDrawMode(Renderer::WIRE);
    else if (action == mTransAction)
        renderer->setDrawMode(Renderer::FACES);
    else
        renderer->setDrawMode(Renderer::MULTI);*/
}
