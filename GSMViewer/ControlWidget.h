#pragma once

#include <qdockwidget.h>
#include "ui_ControlWidget.h"
#include "RoadGraph.h"

class MainWindow;
class GLWidget;
class RoadGraphEditor;

class ControlWidget : public QDockWidget {
Q_OBJECT

private:
	Ui::ControlWidget ui;
	MainWindow* mainWin;

public:
	ControlWidget(MainWindow* mainWin);

	void setRoadVertex(RoadVertexDesc vertexDesc, RoadVertexPtr selectedVertex);
	void setRoadEdge(RoadEdgePtr selectedEdge);

public slots:
	void clean();
	void simplify();
	void reduce();
	void removeShortDeadend();
	void extractMajorRoads();
	void connectRoads();
	void planarify();
};

