#include "MainWidget.h"

#include <QtWidgets\qboxlayout.h>


#include <QtGui\qevent.h>


MainWidget::MainWidget()
{
	
	QHBoxLayout * hLayout;
	this->setLayout(hLayout = new QHBoxLayout);
	QVBoxLayout * vLayout;
	hLayout->addLayout(vLayout = new QVBoxLayout);

	newGL = new MyGLWidget;
	hLayout->addWidget(newGL);
	
	resize(1200, 800);
}


MainWidget::~MainWidget()
{
	delete newGL;
}

bool MainWidget::event(QEvent *e)
{
	if (e->type() == QEvent::KeyPress)
	{
		newGL->eventWrapper(e);
		return true;
	}
	if (e->type() == QEvent::Close)
		isRunning = false;
	return QWidget::event(e);
}