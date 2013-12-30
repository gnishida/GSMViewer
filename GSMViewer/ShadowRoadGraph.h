#pragma once

#include "RoadGraph.h"
#include <QVector2D>

class ShadowRoadGraph {
public:
	RoadGraph* roads;
	RoadGraph* roadsOrig;
	int type;
	QVector2D center;
	float rotation;
	QVector2D translation;

public:
	ShadowRoadGraph(RoadGraph* roads, RoadGraph* roadsOrig, int type, const QVector2D& center, float rotation, const QVector2D& translation);
	~ShadowRoadGraph();

	void generateMesh();
	void addMeshFromEdge(RenderablePtr renderable, RoadEdgePtr edge, const QColor& color, float height);

	RoadGraph* instantiateRoads();
};

