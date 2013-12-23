#pragma once

#include "RoadGraph.h"

class ShadowRoadGraph {
public:
	RoadGraph* roads;

public:
	ShadowRoadGraph(RoadGraph* roads);
	~ShadowRoadGraph();

	void generateMesh();
	void addMeshFromEdge(Renderable* renderable, RoadEdge* edge, float widthBase, QColor color, float height);
};

