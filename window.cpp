#include "window.h"

#include <QMenuBar>
#include <QApplication>
#include <QTextStream>
#include "a2.h"

Window::Window(QWidget * parent) :
    QMainWindow(parent)
{
    setWindowTitle("CPSC453: Boxes vs the World!");

    // Create the main drawing object
    renderer = new Renderer();
    renderer->setMinimumSize(400, 400);

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
    // Add viewport rotate, translate, perspective to View submenu
    mViewMenu->addAction(mVRotateAction);
    mViewMenu->addAction(mVTransAction);
    mViewMenu->addAction(mVPerspAction);

    // Add model rotate, translate, scale to Model submenu
    mModelMenu->addAction(mMRotateAction);
    mModelMenu->addAction(mMTransAction);
    mModelMenu->addAction(mMScaleAction);

    mModeMenu->addAction(mViewportAction);  // add viewport editing

    // Setup the mode label
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

    // Rotates the view
    mVRotateAction = new QAction(tr("&Rotate"), this);
    mVRotateAction->setShortcut(QKeySequence(Qt::Key_O));
    mVRotateAction->setStatusTip(tr("Rotates the view"));
    mModeGroup->addAction(mVRotateAction);

    // Translates the view
    mVTransAction = new QAction(tr("&Translate"), this);
    mVTransAction->setShortcut(QKeySequence(Qt::Key_N));
    mVTransAction->setStatusTip(tr("Translates the view"));
    mModeGroup->addAction(mVTransAction);

    // Adjusts the perspective projection
    mVPerspAction = new QAction(tr("&Perspective"), this);
    mVPerspAction->setShortcut(QKeySequence(Qt::Key_P));
    mVPerspAction->setStatusTip(tr("Adjust the perspective"));
    mModeGroup->addAction(mVPerspAction);

    // Rotates the model
    mMRotateAction = new QAction(tr("&Rotate"), this);
    mMRotateAction->setShortcut(QKeySequence(Qt::Key_R));
    mMRotateAction->setStatusTip(tr("Rotates the model"));
    mModeGroup->addAction(mMRotateAction);

    // Translates the model
    mMTransAction = new QAction(tr("&Translate"), this);
    mMTransAction->setShortcut(QKeySequence(Qt::Key_T));
    mMTransAction->setStatusTip(tr("Translates the model"));
    mModeGroup->addAction(mMTransAction);

    // Scales the model
    mMScaleAction = new QAction(tr("&Scale"), this);
    mMScaleAction->setShortcut(QKeySequence(Qt::Key_S));
    mMScaleAction->setStatusTip(tr("Scale the model "));
    mModeGroup->addAction(mMScaleAction);

    // make nice radio buttons
    mVRotateAction->setCheckable(true);
    mVTransAction->setCheckable(true);
    mVPerspAction->setCheckable(true);

    mMRotateAction->setCheckable(true);
    mMTransAction->setCheckable(true);
    mMScaleAction->setCheckable(true);

    // set default radio button
    mMRotateAction->setChecked(true);

    // adjust the viewport window
    mViewportAction = new QAction(tr("&Viewport"), this);
    mViewportAction->setShortcut(QKeySequence(Qt::Key_V));
    mViewportAction->setStatusTip(tr("Adjust the viewport"));
    mModeGroup->addAction(mViewportAction);
}

// Forward the editing mode to the renderer, and updates the mode text label
void Window::setMode(QAction * action)
{
    if (action == mVRotateAction)           // for each possible mode, forward to renderer and update text label
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
