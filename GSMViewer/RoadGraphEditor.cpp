#include "RoadGraphEditor.h"
#include "GraphUtil.h"

RoadGraphEditor::RoadGraphEditor() {
	roads = new RoadGraph();

	clear();
}

RoadGraphEditor::~RoadGraphEditor() {
	clear();
}

/**
 * Clear the road graph, and free all the allocated memory as well.
 */
void RoadGraphEditor::clear() {
	roads->clear();

	selectedVertex = NULL;
	selectedEdge = NULL;
	movingVertex = NULL;

	for (int i = 0; i < history.size(); i++) {
		delete history[i];
	}
}

void RoadGraphEditor::openRoad(QString filename) {
	clear();

	GraphUtil::loadRoads(roads, filename, 7);
}

void RoadGraphEditor::saveRoad(QString filename) {
	GraphUtil::saveRoads(roads, filename);
}

void RoadGraphEditor::undo() {
	if (history.size() == 0) return;

	GraphUtil::copyRoads(history[history.size() - 1], roads);
	history.pop_back();
	roads->setModified();

	selectedVertex = NULL;
	selectedEdge = NULL;
}

bool RoadGraphEditor::deleteEdge() {
	if (selectedEdge == NULL) return false;

	history.push_back(GraphUtil::copyRoads(roads));
	roads->graph[selectedEdgeDesc]->valid = false;
	selectedEdge = NULL;
	roads->setModified();

	return true;
}

void RoadGraphEditor::simplify(float threshold) {
	history.push_back(GraphUtil::copyRoads(roads));
	GraphUtil::simplify(roads, threshold);
}

bool RoadGraphEditor::selectVertex(const QVector2D& pt) {
	if (roads == NULL) return false;

	RoadVertexDesc desc;
	if (!GraphUtil::getVertex(roads, pt, roads->widthBase, desc)) {
		selectedVertex = NULL;
		return false;
	}

	selectedVertexDesc = desc;
	selectedVertex = roads->graph[desc];

	selectedEdge = NULL;

	return true;
}

bool RoadGraphEditor::selectEdge(const QVector2D& pt) {
	if (roads == NULL) return false;

	RoadEdgeDesc desc;
	if (!GraphUtil::getEdge(roads, pt, roads->widthBase, desc)) {
		selectedEdge = NULL;
		return false;
	}

	selectedEdgeDesc = desc;
	selectedEdge = roads->graph[selectedEdgeDesc];

	return true;
}

void RoadGraphEditor::moveSelectedVertex(const QVector2D& pt) {
	if (roads == NULL) return;

	if (movingVertex == NULL) {
		history.push_back(GraphUtil::copyRoads(roads));
		movingVertex = selectedVertex;
	}
	
	GraphUtil::moveVertex(roads, selectedVertexDesc, pt);
}

void RoadGraphEditor::moveSelectedVertex(const QVector2D& pt, float snap_threshold) {
	if (roads == NULL) return;

	if (movingVertex == NULL) {
		history.push_back(GraphUtil::copyRoads(roads));
		movingVertex = selectedVertex;
	}
	
	// if there is a vertex close to this point, snap to it.
	RoadVertexDesc desc;
	if (GraphUtil::getVertex(roads, pt, snap_threshold, selectedVertexDesc, desc)) {
		GraphUtil::moveVertex(roads, selectedVertexDesc, roads->graph[desc]->pt);
	} else {
		GraphUtil::moveVertex(roads, selectedVertexDesc, pt);
	}
}

void RoadGraphEditor::stopMovingSelectedVertex() {
	movingVertex = NULL;
}

void RoadGraphEditor::stopMovingSelectedVertex(float snap_threshold) {
	if (movingVertex != NULL) {
		// if there is a vertex close to this point, snap to it.
		RoadVertexDesc desc;
		if (GraphUtil::getVertex(roads, roads->graph[selectedVertexDesc]->pt, snap_threshold, selectedVertexDesc, desc)) {
			GraphUtil::snapVertex(roads, selectedVertexDesc, desc);
		}
	}

	movingVertex = NULL;
}
