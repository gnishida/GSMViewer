#pragma once

#include "RoadGraph.h"

class ShadowRoadGraph {
public:
	RoadGraph* roads;
	QVector2D center;

public:
	ShadowRoadGraph(RoadGraph* roads, const QVector2D center);
	~ShadowRoadGraph();

	void generateMesh();
	void addMeshFromEdge(Renderable* renderable, RoadEdge* edge, QColor color, float height);
};

