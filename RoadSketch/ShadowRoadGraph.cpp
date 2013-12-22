#include "ShadowRoadGraph.h"

ShadowRoadGraph::ShadowRoadGraph(RoadGraph* roads) {
	this->roads = roads;
}

ShadowRoadGraph::~ShadowRoadGraph() {
}

void ShadowRoadGraph::generateMesh() {
	if (!roads->modified) return;

	roads->renderables.clear();

	// road edge
	RoadEdgeIter ei, eend;
	for (boost::tie(ei, eend) = boost::edges(roads->graph); ei != eend; ++ei) {
		if (!roads->graph[*ei]->valid) continue;

		RoadEdge* edge = roads->graph[*ei];

		QColor color = QColor(196, 196, 196);

		Renderable renderable(GL_LINE_STRIP);
		// draw the border of the road segment
		addMeshFromEdge(&renderable, edge, roads->widthBase, color, 0.0f);

		roads->renderables.push_back(renderable);
	}

	roads->modified = false;
}

/**
 * Add a mesh for the specified edge.
 */
void ShadowRoadGraph::addMeshFromEdge(Renderable* renderable, RoadEdge* edge, float width, QColor color, float height) {
	Vertex v;

	v.color[0] = color.redF();
	v.color[1] = color.greenF();
	v.color[2] = color.blueF();
	v.color[3] = color.alphaF();
	v.normal[0] = 0.0f;
	v.normal[1] = 0.0f;
	v.normal[2] = 1.0f;

	v.location[2] = height;

	int num = edge->polyLine.size();

	// draw the edge
	for (int i = 0; i < num; ++i) {
		v.location[0] = edge->polyLine[i].x();
		v.location[1] = edge->polyLine[i].y();
		renderable->vertices.push_back(v);
	}
}
