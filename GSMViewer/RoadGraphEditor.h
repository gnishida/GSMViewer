#pragma once

#include "RoadGraph.h"
#include "BBox.h"
#include "ClipBoard.h"
#include "VoronoiDiagram.h"
#include <boost/polygon/voronoi.hpp>

class RoadGraphEditor {
public:
	static enum {
		MODE_DEFAULT = 0,
		MODE_VERTEX_SELECTED,
		MODE_VERTEX_MOVING,
		MODE_EDGE_SELECTED,
		MODE_DEFINING_AREA,
		MODE_AREA_SELECTED,
		MODE_AREA_MOVING,
		MODE_AREA_RESIZING_TL,
		MODE_AREA_RESIZING_TR,
		MODE_AREA_RESIZING_BL,
		MODE_AREA_RESIZING_BR,
		MODE_AREA_DISTORTING_TL,
		MODE_AREA_DISTORTING_TR,
		MODE_AREA_DISTORTING_BL,
		MODE_AREA_DISTORTING_BR,
	};

public:
	RoadGraph* roads;
	RoadGraph* roadsOrig;

	int mode;

	AbstractAreaPtr selectedArea;				// the selected area
	RoadVertexPtr selectedVertex;
	RoadVertexDesc selectedVertexDesc;
	RoadEdgePtr selectedEdge;
	RoadEdgeDesc selectedEdgeDesc;

	RoadGraph* selectedRoads;
	RoadGraph* selectedRoadsOrig;
	std::vector<RoadGraph*> interpolatedRoads;

	VoronoiDiagram voronoiDiagram;

	ClipBoard clipBoard;

	std::vector<RoadGraph*> history;

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

	// MODE_DEFAULT DEFINING_AREA
	void startDefiningArea(const QVector2D& pt);
	void defineArea(const QVector2D& pt);
	void stopDefiningArea();

	// MODE_AREA_DISTORTING
	void startDistortingArea(int type);
	void distortArea(const QVector2D& pt);
	void stopDistortingArea();

	// MODE_AREA_MOVING
	void startMovingArea();
	void moveArea(float dx, float dy);
	void stopMovingArea();

	// MODE_AREA_RESIZING_XX
	void startResizingArea(int type);
	void resizeArea(const QVector2D& pt);
	void stopResizingArea();

	bool selectVertex(const QVector2D& pt);
	bool selectEdge(const QVector2D& pt);

	// MODE_VERTEX_SELECTED | MODE_VERTEX_MOVING
	void startMovingVertex();
	void moveVertex(const QVector2D& pt);
	void moveVertex(const QVector2D& pt, float snap_threshold);
	void stopMovingVertex();
	void stopMovingVertex(float snap_threshold);

	void unselectRoads();
	void connectRoads();
	void interpolate();
	void showInterpolatedRoads(int ratio);
	void finalizeInterpolation(int ratio);
	bool splitEdge(const QVector2D& pt);

	// Voronoi
	void voronoi();
	void voronoiMerge();
	void voronoiMerge2();
	void voronoiMerge2(const AbstractArea& area);
	void voronoiMerge3();

	// Simple connect
	void simpleConnect();
};

