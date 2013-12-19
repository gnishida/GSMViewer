#pragma once

#include "RoadGraph.h"
#include "BBox.h"

class RoadGraphEditor {
public:
	RoadGraph* roads;

	BBox* bbox;
	RoadVertex* selectedVertex;
	RoadVertexDesc selectedVertexDesc;
	RoadEdge* selectedEdge;
	RoadEdgeDesc selectedEdgeDesc;

	std::vector<RoadGraph*> history;
	RoadVertex* movingVertex;

public:
	RoadGraphEditor();
	~RoadGraphEditor();

	void clear();
	void openRoad(QString filename);
	void saveRoad(QString filename);
	void undo();
	void cut();
	bool deleteEdge();
	void simplify(float threshold);
	void reduce();
	void removeShortDeadend(float threshold);

	void selectAll();
	void startSelection(const QVector2D& pt);
	void updateSelection(const QVector2D& pt);
	void endSelection();
	void moveSelection(float dx, float dy);
	bool selectVertex(const QVector2D& pt);
	bool selectEdge(const QVector2D& pt);
	void moveSelectedVertex( const QVector2D& pt);
	void moveSelectedVertex( const QVector2D& pt, float snap_threshold);
	void stopMovingSelectedVertex();
	void stopMovingSelectedVertex(float snap_threshold);
};

