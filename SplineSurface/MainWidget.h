#ifndef MAIN_WIDGET_H
#define MAIN_WIDGET_H

#include <QtWidgets\qwidget.h>
#include "MyGLWidget.h"

class MainWidget :
	public QWidget
{
public:
	MainWidget();
	~MainWidget();

	bool isRunning = true;
	void updateGLWidget(float deltaTime){ newGL->updateWidget(deltaTime); }
	void repaintGLWidget(){ newGL->repaint(); }

private:

	MyGLWidget* newGL;
	bool event(QEvent *e);
};

#endif // !MAIN_WIDGET_H