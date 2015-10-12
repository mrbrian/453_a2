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
                             double near, double far)
{
    Matrix4x4 m_projection;
    // Fill me in!
    m_projection[0][0] = 1 / (aspect * atan(fov / 2));
    m_projection[1][1] = 1 / atan(fov / 2);
    m_projection[2][2] = -(far + near) / (far - near);
    m_projection[2][3] = -2 * (far * near) / (far - near);
    m_projection[3][2] = 1;
    m_projection[3][3] = 0;

    return m_projection;
}

int perspMatrix()
{
    Matrix4x4 p = perspective(30.0 / 180 * M_PI, 1.33, 1, 10000);
    Vector3D result = *new Vector3D(1,0,10);
    Vector3D expected = *new Vector3D();
    result = p * result;

    return 0;
}

float tests()
{
    double f = atan(1);
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
    mModeMenu->addAction(mRotateAction);
    mModeMenu->addAction(mTransAction);
    mModeMenu->addAction(mPerspAction);
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

    mRotateAction = new QAction(tr("&Rotate"), this);
    mRotateAction->setShortcut(QKeySequence(Qt::Key_O));
    mRotateAction->setStatusTip(tr("Rotates the view"));
    mModeGroup->addAction(mRotateAction);

    mTransAction = new QAction(tr("&Translate"), this);
    mTransAction->setShortcut(QKeySequence(Qt::Key_N));
    mTransAction->setStatusTip(tr("Translates the view"));
    mModeGroup->addAction(mTransAction);

    mPerspAction = new QAction(tr("&Perspective"), this);
    mPerspAction->setShortcut(QKeySequence(Qt::Key_P));
    mPerspAction->setStatusTip(tr("Adjust the perspective"));
    mModeGroup->addAction(mPerspAction);

    mRotateAction->setCheckable(true);
    mTransAction->setCheckable(true);
    mPerspAction->setCheckable(true);
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
