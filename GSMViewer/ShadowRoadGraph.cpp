#include "ShadowRoadGraph.h"
#include "RoadGraphDatabase.h"
#include "GraphUtil.h"
#include "CircleArea.h"

ShadowRoadGraph::ShadowRoadGraph(RoadGraph* roads, RoadGraph* roadsOrig, int type, const QVector2D& center, float rotation, const QVector2D& translation) {
	this->roads = roads;
	this->roadsOrig = roadsOrig;
	this->type = type;
	this->center = center;
	this->rotation = rotation;
	this->translation = translation;

	// translation and rotation
	GraphUtil::rotate(roads, -rotation, center);
	GraphUtil::translate(roads, translation);
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

		RoadEdgePtr edge = roads->graph[*ei];

		RoadVertexDesc src = boost::source(*ei, roads->graph);
		RoadVertexDesc tgt = boost::target(*ei, roads->graph);

		QColor color;
		float ratio = ((roads->graph[src]->pt + roads->graph[tgt]->pt) / 2.0f - center - translation).length() / 1000.0f;
		if (ratio > 1.0f) ratio = 1.0f;
		color.setRed((int)((233 - 148) * ratio + 148));
		color.setGreen((int)((229 - 148) * ratio + 148));
		color.setBlue((int)((220 - 148) * ratio + 148));
		if (edge->fullyPaired) {
			color = QColor(255, 0, 0);
		}

		RenderablePtr renderable = RenderablePtr(new Renderable(GL_LINE_STRIP, 1.0f));
		addMeshFromEdge(renderable, edge, color, 0.0f);

		roads->renderables.push_back(renderable);
	}

	roads->modified = false;
}

/**
 * Add a mesh for the specified edge.
 */
void ShadowRoadGraph::addMeshFromEdge(RenderablePtr renderable, RoadEdgePtr edge, const QColor& color, float height) {
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

RoadGraph* ShadowRoadGraph::instantiateRoads() {
	RoadGraph* new_roads = GraphUtil::copyRoads(roadsOrig);

	// translation and rotation
	GraphUtil::rotate(new_roads, -rotation, center);
	GraphUtil::translate(new_roads, translation);

	if (type == RoadGraphDatabase::TYPE_LARGE) {
		CircleArea area(center + translation, 2400.0f);
		GraphUtil::extractRoads2(new_roads, area);
	} else {
		CircleArea area(center + translation, 800.0f);
		GraphUtil::extractRoads2(new_roads, area);
	}

	return new_roads;
}
