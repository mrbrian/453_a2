#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>
#include <QMenu>
#include <QLabel>
#include <QVBoxLayout>
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
    QAction * mResetAction;     // reset the all transforms & settings
    QAction * mQuitAction;      // quit the program

    QActionGroup * mModeGroup;  // checkbox group
    QMenu * mModeMenu;          // mode menu
    QMenu * mModelMenu;         // model sub-menu
    QMenu * mViewMenu;          // view sub-menu
    QAction * mVRotateAction;   // Rotate view
    QAction * mVTransAction;    // Translate view
    QAction * mVPerspAction;    // View perspective

    QAction * mMRotateAction;   // Rotate model
    QAction * mMTransAction;    // Translate model
    QAction * mMScaleAction;    // Scale model

    QAction * mViewportAction;  // Viewport

    QLabel * modeLabel;         // current model label

    // helper function for creating actions
	void createActions();
};

#endif
