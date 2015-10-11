#include "window.h"

#include <QMenuBar>
#include <QApplication>

Window::Window(QWidget * parent) :
    QMainWindow(parent)
{
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
	mFileMenu->addAction(mQuitAction);  // add quitting
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
}
