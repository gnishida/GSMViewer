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
	void addMeshFromEdge(RenderablePtr renderable, RoadEdgePtr edge, const QColor& color, float height);
};

