#include "RoadGraphEditor.h"
#include "GraphUtil.h"

RoadGraphEditor::RoadGraphEditor() {
	roads = new RoadGraph();

	clear();
}

RoadGraphEditor::~RoadGraphEditor() {
}

void RoadGraphEditor::clear() {
	roads->clear();

	selectedVertex = NULL;
	selectedEdge = NULL;
}

void RoadGraphEditor::openRoad(QString filename) {
	clear();

	GraphUtil::loadRoads(roads, filename, 7);
}

void RoadGraphEditor::saveRoad(QString filename) {
	GraphUtil::saveRoads(roads, filename);
}

void RoadGraphEditor::undo() {
}

bool RoadGraphEditor::deleteEdge() {
	if (selectedEdge == NULL) return false;

	roads->graph[selectedEdgeDesc]->valid = false;
	selectedEdge = NULL;
	roads->setModified();

	return true;
}

bool RoadGraphEditor::selectVertex(const QVector2D& pt) {
	if (roads == NULL) return false;

	RoadVertexDesc v_desc;
	if (!GraphUtil::getVertex(roads, pt, roads->widthBase, v_desc)) return false;

	selectedVertex = roads->graph[v_desc];
	selectedEdge = NULL;

	return true;
}

bool RoadGraphEditor::selectEdge(const QVector2D& pt) {
	if (roads == NULL) return false;

	selectedVertex = NULL;
	RoadEdgeDesc e_desc;
	if (!GraphUtil::getEdge(roads, pt, roads->widthBase, e_desc)) return false;

	selectedEdge = roads->graph[e_desc];
	selectedEdgeDesc = e_desc;

	return true;
}
