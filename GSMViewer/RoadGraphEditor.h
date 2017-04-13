#pragma once

#include <common/BBox.h>
#include <common/Polygon2D.h>
#include <road/RoadGraph.h>

class RoadGraphEditor {
public:
	static enum {
		MODE_DEFAULT = 0,
		MODE_VERTEX_SELECTED,
		MODE_VERTEX_MOVING,
		MODE_EDGE_SELECTED,
	};

public:
	RoadGraph roads;
	RoadGraph roadsOrig;

	int mode;

	RoadVertexPtr selectedVertex;
	RoadVertexDesc selectedVertexDesc;
	RoadEdgePtr selectedEdge;
	RoadEdgeDesc selectedEdgeDesc;

	std::vector<RoadGraph> history;

public:
	RoadGraphEditor();
	~RoadGraphEditor();

	void clear();
	void openRoad(QString filename);
	void saveRoad(QString filename);
	void undo();
	bool deleteEdge();
	void reduce();
	void removeShortDeadend(float threshold);
	void planarify();

	void selectAll();

	void unselectRoads();

	bool selectVertex(const QVector2D& pt);
	bool selectEdge(const QVector2D& pt);

	// MODE_VERTEX_SELECTED | MODE_VERTEX_MOVING
	void startMovingVertex();
	void moveVertex(const QVector2D& pt);
	void moveVertex(const QVector2D& pt, float snap_threshold);
	void stopMovingVertex();
	void stopMovingVertex(float snap_threshold);

	bool splitEdge(const QVector2D& pt);
};

