#pragma once

#include "RoadGraph.h"

class RoadGraphEditor {
public:
	RoadGraph* roads;

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
	bool deleteEdge();
	void simplify(float threshold);

	bool selectVertex(const QVector2D& pt);
	bool selectEdge(const QVector2D& pt);
	void moveSelectedVertex( const QVector2D& pt);
	void moveSelectedVertex( const QVector2D& pt, float snap_threshold);
	void stopMovingSelectedVertex();
	void stopMovingSelectedVertex(float snap_threshold);
};

