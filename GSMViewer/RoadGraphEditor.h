#pragma once

#include "RoadGraph.h"
#include "BBox.h"
#include "ClipBoard.h"
#include <boost/polygon/voronoi.hpp>

class VoronoiVertex {
public:
	RoadGraph* roads;
	RoadVertexDesc desc;

public:
	VoronoiVertex(RoadGraph* roads, RoadVertexDesc desc) {
		this->roads = roads;
		this->desc = desc;
	}
};

namespace boost {
namespace polygon {
template <>
struct geometry_concept<VoronoiVertex> { typedef point_concept type; };
  
template <>
struct point_traits<VoronoiVertex> {
	typedef int coordinate_type;
   
	static inline coordinate_type get(const VoronoiVertex& v, orientation_2d orient) {
		return (orient == HORIZONTAL) ? v.roads->graph[v.desc]->pt.x() * 100 : v.roads->graph[v.desc]->pt.y() * 100;
	}
};
}
}

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

	RoadGraph voronoiGraph;

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
	void voronoi();
	void voronoiCut();
	void voronoiCut2();
	bool isWithinTerritory(RoadGraph* roads1, const QVector2D& center1, RoadGraph* roads2, const QVector2D& center2, const VoronoiVertex& pt);
};

