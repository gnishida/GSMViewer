#pragma once

#include "RoadGraph.h"
#include "Sketch.h"
#include "BBox.h"
#include "ClipBoard.h"
#include "RoadGraphDatabase.h"
#include "ShadowRoadGraph.h"
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
	static enum {
		MODE_BASIC = 0,
		MODE_BASIC_VERTEX_SELECTED,
		MODE_BASIC_VERTEX_MOVING,
		MODE_BASIC_EDGE_SELECTED,
		MODE_BASIC_DEFINING_AREA,
		MODE_BASIC_AREA_SELECTED,
		MODE_BASIC_RESIZING_AREA_BR,
		MODE_BASIC_DISTORTING_AREA,

		MODE_LAYER,
		MODE_LAYER_SELECTED,
		MODE_LAYER_MOVING,
		MODE_LAYER_ROTATING,
		MODE_LAYER_DISTORTING,
		MODE_LAYER_SCALING,

		MODE_SKETCH,
		MODE_SKETCH_SKETCHING,
	};

public:
	RoadGraph* roads;

	int mode;

	AbstractArea* selectedArea;				// the selected area
	RoadVertexPtr selectedVertex;
	RoadVertexDesc selectedVertexDesc;
	RoadEdgePtr selectedEdge;
	RoadEdgeDesc selectedEdgeDesc;

	RoadGraph* selectedRoads;
	RoadGraph* selectedRoadsOrig;
	std::vector<RoadGraph*> interpolatedRoads;

	Sketch sketch;
	RoadGraphDatabase* roadDB;
	QList<ShadowRoadGraph*> shadowRoads;

	RoadGraph voronoiGraph;

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

	// MODE_BASIC_VERTEX_SELECTED | MODE_BASIC_VERTEX_MOVING
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
	void voronoi();
	void voronoiCut();
	void voronoiCut2();
	bool isWithinTerritory(RoadGraph* roads1, const QVector2D& center1, RoadGraph* roads2, const QVector2D& center2, const VoronoiVertex& pt);

	// Sketch
	void startSketchLine(const QVector2D& pt, float snap_threshold);
	void finalizeSketchLine(float simplify_threshold, float snap_threshold);
	void instanciateShadowRoads();
};

