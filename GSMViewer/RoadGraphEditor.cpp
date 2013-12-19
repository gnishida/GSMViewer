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

	bbox = NULL;
	selectedVertex = NULL;
	selectedEdge = NULL;
	movingVertex = NULL;
	clipBoard = NULL;

	for (int i = 0; i < history.size(); i++) {
		delete history[i];
	}
	history.clear();
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

void RoadGraphEditor::cut() {
	if (bbox == NULL) return;

	history.push_back(GraphUtil::copyRoads(roads));
	GraphUtil::subtractRoads(roads, *bbox);
}

void RoadGraphEditor::copy() {
	if (bbox == NULL) return;

	//clipBoard->copy(bbox, GraphUtil::
}

void RoadGraphEditor::paste() {
	if (bbox == NULL) return;

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

void RoadGraphEditor::reduce() {
	history.push_back(GraphUtil::copyRoads(roads));
	GraphUtil::reduce(roads);
}

void RoadGraphEditor::removeShortDeadend(float threshold) {
	history.push_back(GraphUtil::copyRoads(roads));
	GraphUtil::removeShortDeadend(roads, threshold);
}

void RoadGraphEditor::selectAll() {
	if (bbox != NULL) delete bbox;

	bbox = new BBox(GraphUtil::getAABoundingBox(roads));
}

void RoadGraphEditor::startSelection(const QVector2D& pt) {
	if (bbox != NULL) delete bbox;

	bbox = new BBox();
	bbox->addPoint(pt);
}

void RoadGraphEditor::updateSelection(const QVector2D& pt) {
	bbox->maxPt.setX(pt.x());
	bbox->maxPt.setY(pt.y());
}

void RoadGraphEditor::endSelection() {
	if (bbox->maxPt.x() < bbox->minPt.x()) {
		float x = bbox->maxPt.x();
		bbox->maxPt.setX(bbox->minPt.x());
		bbox->minPt.setX(x);
	}
	if (bbox->maxPt.y() < bbox->minPt.y()) {
		float y = bbox->maxPt.y();
		bbox->maxPt.setY(bbox->minPt.y());
		bbox->minPt.setY(y);
	}

	// if the box is just a single point, cancel the selection.
	if (bbox->maxPt.x() == bbox->minPt.x() && bbox->maxPt.y() == bbox->minPt.y()) {
		delete bbox;
		bbox = NULL;
	}
}

void RoadGraphEditor::moveSelection(float dx, float dy) {
	bbox->minPt.setX(bbox->minPt.x() + dx);
	bbox->minPt.setY(bbox->minPt.y() + dy);
	bbox->maxPt.setX(bbox->maxPt.x() + dx);
	bbox->maxPt.setY(bbox->maxPt.y() + dy);


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

	// clear the selction for other items
	selectedEdge = NULL;
	if (bbox != NULL) {
		delete bbox;
		bbox = NULL;
	}

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

	// clear the selction for other items
	selectedVertex = NULL;
	if (bbox != NULL) {
		delete bbox;
		bbox = NULL;
	}


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
