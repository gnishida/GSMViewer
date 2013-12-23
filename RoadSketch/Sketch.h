#pragma once

#include "RoadGraph.h"

class Sketch : public RoadGraph {
public:
	RoadVertexDesc currentVertex;
	RoadEdgeDesc currentEdge;

public:
	Sketch();
	~Sketch();

	void generateMesh();
	void addMeshFromEdge(Renderable* renderable, RoadEdge* edge, QColor color, float height);
};

