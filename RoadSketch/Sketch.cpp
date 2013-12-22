#include "Sketch.h"

Sketch::Sketch() {
	modified = true;

	renderable = new Renderable(GL_LINE_STRIP);
}

Sketch::~Sketch() {
}

void Sketch::generateMesh() {
	if (!modified) return;

	//renderables.clear();
	renderable->vertices.clear();

	// road edge
	RoadEdgeIter ei, eend;
	for (boost::tie(ei, eend) = boost::edges(graph); ei != eend; ++ei) {
		if (!graph[*ei]->valid) continue;

		RoadEdge* edge = graph[*ei];

		QColor color = QColor(0, 0, 255);

		addMeshFromEdge(renderable, edge, widthBase, color, 0.0f);
	}

	modified = false;
}

void Sketch::addMeshFromEdge(Renderable* renderable, RoadEdge* edge, float width, QColor color, float height) {
	Vertex v;

	v.color[0] = color.redF();
	v.color[1] = color.greenF();
	v.color[2] = color.blueF();
	v.color[3] = color.alphaF();
	v.normal[0] = 0.0f;
	v.normal[1] = 0.0f;
	v.normal[2] = 1.0f;

	int num = edge->polyLine.size();

	// draw the edge
	for (int i = 0; i < num; ++i) {
		v.location[0] = edge->polyLine[i].x();
		v.location[1] = edge->polyLine[i].y();
		v.location[2] = height;

		renderable->vertices.push_back(v);
	}
}
