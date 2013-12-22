#pragma once

#include "RoadGraph.h"
#include "BBox.h"
#include "ClipBoard.h"

class RoadGraphEditor {
public:
	static enum { MODE_DEFAULT = 0, MODE_VERTEX_SELECTED, MODE_EDGE_SELECTED, MODE_DEFINING_AREA, MODE_AREA_SELECTED, MODE_DISTORTING_AREA, MODE_RESIZING_AREA_BR };

public:
	RoadGraph* roads;

	int mode;

	AbstractArea* selectedArea;				// the selected area
	RoadVertex* selectedVertex;
	RoadVertexDesc selectedVertexDesc;
	RoadEdge* selectedEdge;
	RoadEdgeDesc selectedEdgeDesc;

	RoadGraph* selectedRoads;
	RoadGraph* selectedRoadsOrig;
	std::vector<RoadGraph*> interpolatedRoads;

	ClipBoard* clipBoard;

	std::vector<RoadGraph*> history;
	RoadVertex* movingVertex;

public:
	RoadGraphEditor();
	~RoadGraphEditor();

	void clear();
	void clearInterpolatedRoads();
	void openRoad(QString filename);
	void openToAddRoad(QString filename);
	void saveRoad(QString filename);
	void undo();
	void cut();
	void copy();
	void paste();
	bool deleteEdge();
	void simplify(float threshold);
	void reduce();
	void removeShortDeadend(float threshold);

	void selectAll();
	void startArea(const QVector2D& pt);
	void updateArea(const QVector2D& pt);
	void finalizeArea();
	void resizeAreaBR(const QVector2D& pt);
	void startDistortingArea();
	void distortArea(float dx, float dy);
	void finalizeDistortArea();
	void moveArea(float dx, float dy);
	bool selectVertex(const QVector2D& pt);
	bool selectEdge(const QVector2D& pt);
	void moveSelectedVertex(const QVector2D& pt);
	void moveSelectedVertex(const QVector2D& pt, float snap_threshold);
	void stopMovingSelectedVertex();
	void stopMovingSelectedVertex(float snap_threshold);
	void unselectRoads();
	void connectRoads();
	void interpolate();
	void showInterpolatedRoads(int ratio);
	void finalizeInterpolation(int ratio);
	bool splitEdge(const QVector2D& pt);
};
