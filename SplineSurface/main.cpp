#include <QtWidgets/qapplication.h>
#include "MainWidget.h"
#include <QtCore\qtime>
#include "EsgiTimer.h"

const float FPS = 60.f;

int main(int argc, char ** argv)
{
	bool first = false;
	QApplication app(argc, argv);
	MainWidget glWid;
	glWid.show();
	EsgiTimer timer;
	auto updateTimeStep = 1.f / FPS;
	timer.Begin();
	auto lastUpdate = timer.GetTimerValue();
	while (glWid.isRunning) {
		app.processEvents();
		if (first == false)
		{
			lastUpdate = timer.GetTimerValue();
			first = true;
		}
		auto currentTime = timer.GetTimerValue();
		auto timeSinceLastUpdate = timer.GetElapsedTimeSince(lastUpdate);
		while (timeSinceLastUpdate>updateTimeStep){
			glWid.updateGLWidget(timeSinceLastUpdate);
			glWid.repaintGLWidget();
			timeSinceLastUpdate -= updateTimeStep;
			lastUpdate = timer.GetTimerValue();
		}
	}
	return app.exec();
}