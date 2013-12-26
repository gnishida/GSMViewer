#include "Sketch.h"
#include "GraphUtil.h"

Sketch::Sketch() {
	modified = true;
}

Sketch::~Sketch() {
}

void Sketch::generateMesh() {
	if (!modified) return;

	renderables.clear();

	// road edges
	RoadEdgeIter ei, eend;
	for (boost::tie(ei, eend) = boost::edges(graph); ei != eend; ++ei) {
		if (!graph[*ei]->valid) continue;

		RoadEdgePtr edge = graph[*ei];

		QColor color = QColor(0, 0, 255);

		RenderablePtr renderable = RenderablePtr(new Renderable(GL_LINE_STRIP, 3.0f));
		addMeshFromEdge(renderable, edge, color, 1.0f);
		renderables.push_back(renderable);
	}

	// road vertices
	RenderablePtr renderable = RenderablePtr(new Renderable(GL_POINTS, 10.0f));
	Vertex v;

	v.color[0] = 0;
	v.color[1] = 0;
	v.color[2] = 1.0f;
	v.color[3] = 1.0f;
	v.normal[0] = 0.0f;
	v.normal[1] = 0.0f;
	v.normal[2] = 1.0f;

	RoadVertexIter vi, vend;
	for (boost::tie(vi, vend) = boost::vertices(graph); vi != vend; ++vi) {
		if (!graph[*vi]->valid) continue;

		v.location[0] = graph[*vi]->pt.x();
		v.location[1] = graph[*vi]->pt.y();
		v.location[2] = 1.0f;
		renderable->vertices.push_back(v);
	}
	renderables.push_back(renderable);

	modified = false;
}

void Sketch::addMeshFromEdge(RenderablePtr renderable, RoadEdgePtr edge, const QColor& color, float height) {
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

void Sketch::startLine(const QVector2D& pt, float snap_threshold) {
	RoadVertexDesc v1_desc;
	if (!GraphUtil::getVertex(this, pt, snap_threshold, v1_desc)) {
		RoadEdgeDesc e_desc;
		if (GraphUtil::getEdge(this, pt, snap_threshold, e_desc)) {
			// if there is an edge close to it, snap it onto the edge
			v1_desc = GraphUtil::splitEdge(this, e_desc, pt);
		} else {
			RoadVertexPtr v1 = RoadVertexPtr(new RoadVertex(pt));
			v1_desc = boost::add_vertex(graph);
			graph[v1_desc] = v1;
		}
	}

	RoadVertexPtr v2 = RoadVertexPtr(new RoadVertex(pt));
	RoadVertexDesc v2_desc = boost::add_vertex(graph);
	graph[v2_desc] = v2;

	currentVertex = v2_desc;
	currentEdge = GraphUtil::addEdge(this, v1_desc, v2_desc, 1, 1, false);
}

void Sketch::addPointToLine(const QVector2D& pt) {
	if ((pt - graph[currentVertex]->pt).length() > 3.0f) {
		graph[currentVertex]->pt = pt;
		graph[currentEdge]->polyLine.push_back(pt);
		setModified();
	}
}

void Sketch::finalizeLine(float simplify_threshold, float snap_threshold) {
	graph[currentEdge]->polyLine = GraphUtil::simplifyPolyLine(graph[currentEdge]->polyLine, simplify_threshold);

	if (graph[currentEdge]->getLength() > snap_threshold) {
		RoadVertexDesc v_desc;
		if (GraphUtil::getVertex(this, currentVertex, snap_threshold, v_desc)) {
			// if there is a vertex close to it, snap it to the vertex
			GraphUtil::snapVertex(this, currentVertex, v_desc);
		} else {
			RoadEdgeDesc e_desc;
			if (GraphUtil::getEdge(this, currentVertex, snap_threshold, e_desc)) {
				// if there is an edge close to it, snap it onto the edge
				v_desc = GraphUtil::splitEdge(this, e_desc, graph[currentVertex]->pt);
				GraphUtil::snapVertex(this, currentVertex, v_desc);
			}
		}

		GraphUtil::planarify(this);
		GraphUtil::reduce(this);
	} else {
		graph[currentEdge]->valid = false;
	}

	setModified();
}
