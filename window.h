#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>
#include <QMenu>

#include "renderer.h"

class Window : public QMainWindow
{
    // informs the qmake that a Qt moc_* file will need to be generated
    Q_OBJECT

public:
    // constructor
    Window(QWidget *parent = 0);

    // destructor
    ~Window();

protected:

private slots:
    void setMode(QAction * action);
private:
	// The main OpenGL area
    Renderer * renderer;

	// Menu items and actions
    QMenu * mFileMenu;
    QAction * mResetAction;
    QAction * mQuitAction;

    QActionGroup * mModeGroup;
    QMenu * mModeMenu;
    QAction * mRotateAction;
    QAction * mTransAction;
    QAction * mPerspAction;

	// helper function for creating actions
	void createActions();
};

#endif
