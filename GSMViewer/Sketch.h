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
	void addMeshFromEdge(RenderablePtr renderable, RoadEdgePtr edge, const QColor& color, float height);

	void startLine(const QVector2D& pt, float snap_threshold);
	void addPointToLine(const QVector2D& pt);
	void finalizeLine(float simplify_threshold, float snap_threshold);
};

