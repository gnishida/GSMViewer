#pragma once

#include <qdockwidget.h>
#include "ui_ControlWidget.h"
#include "RoadVertex.h"
#include "RoadEdge.h"

class MainWindow;

class ControlWidget : public QDockWidget {
Q_OBJECT

private:
	Ui::ControlWidget ui;
	MainWindow* mainWin;

public:
	ControlWidget(MainWindow* mainWin);

	void setRoadVertex(RoadVertex* selectedVertex);
	void setRoadEdge(RoadEdge* selectedEdge);

public slots:
	void simplify();
};

