#pragma once

#include "RoadGraph.h"

class RoadGraphEditor {
public:
	RoadGraph* roads;

	RoadVertex* selectedVertex;
	RoadEdge* selectedEdge;
	RoadEdgeDesc selectedEdgeDesc;

public:
	RoadGraphEditor();
	~RoadGraphEditor();

	// [File] menu actions
	void clear();
	void openRoad(QString filename);
	void saveRoad(QString filename);

	// [Edit] menu actions
	void undo();
	bool deleteEdge();

	bool selectVertex(const QVector2D& pt);
	bool selectEdge(const QVector2D& pt);
};

